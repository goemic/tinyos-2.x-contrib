/*
 * "Copyright (c) 2008 The Regents of the University  of California.
 * All rights reserved."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 */

#include <string.h>
#include "lib6lowpan.h"
#include "lib6lowpanFrag.h"
/*
 * This file presents an interface for parsing IP and UDP headers in a
 * 6loWPAN packet.  
 *
 * @author Stephen Dawson-Haggerty <stevedh@cs.berkeley.edu>
 */

uint8_t linklocal_prefix [] = {0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t multicast_prefix [] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

ip6_addr_t my_address       = {0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64};
uint8_t globalPrefix = 0;

uint8_t cmpPfx(ip6_addr_t a, uint8_t *pfx) {
  return (a[0] == pfx[0] &&
          a[1] == pfx[1] &&
          a[2] == pfx[2] &&
          a[3] == pfx[3] &&
          a[4] == pfx[4] &&
          a[5] == pfx[5] &&
          a[6] == pfx[6] &&
          a[7] == pfx[7]);
}

int ipv6_addr_suffix_is_long(const ip6_addr_t addr) {
  return (!(addr[8] == 0 &&
            addr[9] == 0 &&
            addr[10] == 0 &&
            addr[11] == 0 &&
            addr[12] == 0 &&
            addr[13] == 0));
}


#define ADDRLEN(rv, mask) \

/*
 * return the length of the compressed fields in buf
 */
int getCompressedLen(packed_lowmsg_t *pkt) {
  // min lenght is DISPATCH + ENCODING
  uint8_t encoding, len = 2;
  uint8_t *buf = getLowpanPayload(pkt);

  if (!(*buf == LOWPAN_HC_LOCAL_PATTERN || *buf == LOWPAN_HC_CRP_PATTERN))
    return 0;

  encoding = *(buf + 1);
  if ((encoding & LOWPAN_IPHC_VTF_MASK) == LOWPAN_IPHC_VTF_INLINE)
    len += 4;
  if ((encoding & LOWPAN_IPHC_NH_MASK) == LOWPAN_IPHC_NH_INLINE)
    len += 1;
  if ((encoding &LOWPAN_IPHC_HLIM_MASK) == LOWPAN_IPHC_HLIM_INLINE)
    len += 1;

  switch ((encoding >> LOWPAN_IPHC_SC_OFFSET) & LOWPAN_IPHC_ADDRFLAGS_MASK) { 
  case LOWPAN_IPHC_ADDR_128: len += 16; break; 
  case LOWPAN_IPHC_ADDR_64: len += 8; break;  
  case LOWPAN_IPHC_ADDR_16: len += 2; break;  
  case LOWPAN_IPHC_ADDR_0: len += 0; break;   
  }
  switch ((encoding >> LOWPAN_IPHC_DST_OFFSET) & LOWPAN_IPHC_ADDRFLAGS_MASK) { 
  case LOWPAN_IPHC_ADDR_128: len += 16; break; 
  case LOWPAN_IPHC_ADDR_64: len += 8; break;  
  case LOWPAN_IPHC_ADDR_16: len += 2; break;  
  case LOWPAN_IPHC_ADDR_0: len += 0; break;   
  }

  if ((encoding & LOWPAN_IPHC_NH_MASK) != LOWPAN_IPHC_NH_INLINE) {
    // figure out how long the next header encoding is
      uint8_t *nh = buf + len;
    if ((*nh & LOWPAN_UDP_DISPATCH) == LOWPAN_UDP_DISPATCH) {
      // are at a udp packet
      len += 1; // add LOWPAN_HCNH
      uint8_t udp_enc = *nh;
      //printf("udp_enc: 0x%x\n", udp_enc);
      if ((udp_enc & LOWPAN_UDP_S_MASK) && (udp_enc & LOWPAN_UDP_D_MASK))
        len += 1;
      else if ((udp_enc & LOWPAN_UDP_S_MASK) || (udp_enc & LOWPAN_UDP_D_MASK))
        len += 3;
      else
        len += 4;
      if ((udp_enc & LOWPAN_UDP_C_MASK) == 0)
        len += 2;
    }
  }

  len += (buf - pkt->data);
  return len;
}

int decompressShortAddress(uint8_t dispatch, uint8_t *s_addr, uint8_t *dest) {
  if ((*s_addr & LOWPAN_IPHC_SHORT_MASK) == 0) {
    // simplest case, just use the appropriate prefix.
    if (dispatch == LOWPAN_HC_LOCAL_PATTERN)
      ip_memcpy(dest, linklocal_prefix, 8);
    else
      ip_memcpy(dest, my_address, 8);
    ip_memclr(dest + 8, 8);
    dest[14] = (*s_addr) & ~LOWPAN_IPHC_SHORT_MASK;
    dest[15] = *(s_addr + 1);
    return 0;
  }
  // otherwise we either have an invalid compression, or else it's a
  // multicast address
  ip_memcpy(dest, multicast_prefix, 8);
  ip_memclr(dest + 8, 8);
  switch (*s_addr & LOWPAN_IPHC_SHORT_LONG_MASK) {
  case LOWPAN_IPHC_HC1_MCAST:
    dest[14] = (*s_addr) & ~LOWPAN_IPHC_SHORT_LONG_MASK;
    dest[15] = *(s_addr + 1);
    break;
  case LOWPAN_IPHC_HC_MCAST:
    dest[1] = ((*s_addr) & LOWPAN_HC_MCAST_SCOPE_MASK) >> LOWPAN_HC_MCAST_SCOPE_OFFSET;

    // we'll just direct map the bottom 9 bits in for the moment,
    // since HC doesn't specify anything that would break this.  In
    // the future, a more complicated mapping is likely.
    dest[14] = (*s_addr) & 0x1;
    dest[15] = *(s_addr + 1);
    break;
  default:
    return 1;
  }
  return 0;
}

int decompressAddress(uint8_t dispatch, uint16_t src, uint8_t addr_flags, 
                       uint8_t **buf, uint8_t *dest) {
  uint8_t *prefix;
  uint16_t tmp;
  int rc = 0;
  if (dispatch == LOWPAN_HC_LOCAL_PATTERN)
    prefix = linklocal_prefix;
  else
    prefix = my_address;

  switch (addr_flags) {
  case LOWPAN_IPHC_ADDR_128:
    ip_memcpy(dest, *buf, 16); 
    *buf += 16;
    break;
  case LOWPAN_IPHC_ADDR_64:
    ip_memcpy(dest, prefix, 8);
    ip_memcpy(dest + 8, *buf, 8);
    *buf += 8;
    break;
  case LOWPAN_IPHC_ADDR_16:
    rc = decompressShortAddress(dispatch, *buf, dest);
    *buf += 2;
    break;
  case LOWPAN_IPHC_ADDR_0:
    ip_memcpy(dest, prefix, 8);
    ip_memclr(dest + 8, 6);
    tmp = hton16(src);
    ip_memcpy(dest + 14, (uint8_t *)&tmp, 2);
    break;
  }
  return rc;
}

/*
 * Unpacks all headers, including any compressed transport headers if
 * there is a compression scheme defined for them.
 *
 * @pkt  - the wrapped struct pointing to the compressed headers
 * @dest - buffer to unpack the headers into
 * @len  - the len of 'dest'
 * @return the number of bytes written to dest, or zero if decompression failed.
 *         should be >= sizeof(struct ip6_hdr)
 */
uint8_t *unpackHeaders(packed_lowmsg_t *pkt, uint8_t *dest, uint16_t len) {
  uint8_t dispatch, encoding;
  uint16_t size, extra_header_length = 0;
  uint8_t *buf = (uint8_t *)getLowpanPayload(pkt);

  // pointers to fields  we may come back to fill in later
  uint8_t *plen, *prot_len, *nxt_hdr;

  // a buffer we can write addresses prefixes and suffexes into.
  // now we don't need to check sizes until we get to next headers
  if (buf == NULL || len < sizeof(struct ip6_hdr)) return NULL;
  len -= sizeof(struct ip6_hdr);

  dispatch = *buf; buf++;
  encoding = *buf; buf++;

  if ((encoding & LOWPAN_IPHC_VTF_MASK) == LOWPAN_IPHC_VTF_INLINE) {
    // copy the inline 4 bytes of fields.
    ip_memcpy(dest, buf, 4);
    buf += 4;
  } else {
    // cler the traffic class and flow label fields, and write the version.
    ip_memclr(dest, 4);
    *dest = IPV6_VERSION << 4;
  }
  dest += 4;

  plen = dest;
  prot_len = dest;
  // payload length field requires some computation...
  dest += 2;

  if ((encoding & LOWPAN_IPHC_NH_MASK) == LOWPAN_IPHC_NH_INLINE) {
    *dest = *buf;
    buf++;
  }
  nxt_hdr = dest;

  dest += 1;
  // otherwise, decompress IPNH compression once we reach the end of
  // the packed data.

  if ((encoding & LOWPAN_IPHC_HLIM_MASK) == LOWPAN_IPHC_HLIM_INLINE) {
    *dest = *buf;
    buf++;
  }
  dest += 1;
  // otherwise, follow instructions for reconstructing hop limit once
  // destination address is known.


  // dest points at the start of the source address IP header field.
  decompressAddress(dispatch, pkt->src,
                    (encoding >> LOWPAN_IPHC_SC_OFFSET) & LOWPAN_IPHC_ADDRFLAGS_MASK,
                    &buf, dest);
  dest += 16;

  decompressAddress(dispatch, pkt->src,
                    (encoding >> LOWPAN_IPHC_DST_OFFSET) & LOWPAN_IPHC_ADDRFLAGS_MASK,
                    &buf, dest);
  dest += 16;
 
  // we're done with the IP headers; time to decompress any compressed
  // headers which follow...  We need to re-check that there's enough
  // buffer to do this.


  if ((encoding & LOWPAN_IPHC_NH_MASK) != LOWPAN_IPHC_NH_INLINE) {
    // time to decode some next header fields
    // we ought to be pointing at the HCNH encoding byte now.
    if ((*buf & LOWPAN_UDP_DISPATCH) == LOWPAN_UDP_DISPATCH) {
      if (len < sizeof(struct udp_hdr)) return NULL;
      len -= sizeof(struct udp_hdr);
      struct udp_hdr *udp = (struct udp_hdr *)dest;
      uint8_t udp_enc = *buf;
      uint8_t dst_shift = 4;

      extra_header_length = sizeof(struct udp_hdr);
      buf += 1;
      // UDP
      *nxt_hdr = IANA_UDP;
      if (udp_enc & LOWPAN_UDP_S_MASK) {
        // recover from 4 bit packing
        udp->srcport = hton16((*buf >> 4) + LOWPAN_UDP_PORT_BASE);
        dst_shift = 0;
      } else {
        ip_memcpy((uint8_t *)&udp->srcport, buf, 2);
        buf += 2;
      }

      if (udp_enc & LOWPAN_UDP_D_MASK) {
        udp->dstport = hton16((((*buf >> dst_shift)) & 0x0f) + LOWPAN_UDP_PORT_BASE);
        buf += 1;
      } else {
        if (dst_shift == 0) buf += 1;
        ip_memcpy((uint8_t *)&udp->dstport, buf, 2);
        buf += 2;
      }


      if (udp_enc & LOWPAN_UDP_C_MASK) {
        // we elided the checksum and so are supposed to recompute it here.
        // however, we won't do this.
      } else {
        ip_memcpy((uint8_t *)&udp->chksum, buf, 2);
        buf += 2;
      }

      // still must fill in the length field, but we must first
      // recompute the IP length, which we couldn't do until now.
      // lamers...
      prot_len = (uint8_t *)&udp->len;
      dest += sizeof(struct udp_hdr);


    } else {
      // otherwise who knows what's here... it's an error because the
      // NH bit said we were inline but when we got here, we didn't
      // recognize the NH encoding.
      return NULL;
    }
  }


  // we can go back and figure out the payload length now that we know
  // how long the compressed headers were
  if (hasFrag1Header(pkt) || hasFragNHeader(pkt)) {
    getFragDgramSize(pkt, &size);
    size -= sizeof(struct ip6_hdr);
  } else {
    // it's a one fragment packet
    size = pkt->len - (buf - pkt->data);
    size += extra_header_length;
    size -= getLowpanPayload(pkt) - pkt->data;
  }

  *plen = size >> 8;
  *(plen + 1) = size & 0xff;

  // finally fill in the udp length field that we finally can recompute
  switch (*nxt_hdr) {
  case IANA_UDP:
    *prot_len = size >> 8;
    *(prot_len + 1) = size & 0xff;
  }
  

  return buf;
}

/* packs addr into *buf, and updates the pointer relative to the length
 * that was needed.
 * @returns the bit flags indicating which length was used
 */
uint8_t packAddress(uint8_t dispatch, uint8_t **buf, ip6_addr_t addr) {
  if ((dispatch == LOWPAN_HC_CRP_PATTERN && globalPrefix &&
      cmpPfx(addr, my_address)) ||
      (dispatch == LOWPAN_HC_LOCAL_PATTERN &&
       cmpPfx(addr, linklocal_prefix))) {
    // only choice here are 64-bit and 16-bit addresses
    if (ipv6_addr_suffix_is_long(addr)) {
      // use the 64-bit compression
      ip_memcpy(*buf, &addr[8], 8);
      *buf += 8;
      return LOWPAN_IPHC_ADDR_64;
    } else {
      // down to 16 bits: we never use the 0-bit compression
      // (althought we could for link local).
      ip_memcpy(*buf, &addr[14], 2);
      *buf += 2;
      return LOWPAN_IPHC_ADDR_16;
    }
  } else if (addr[0] == 0xff && // is multicast
             addr[1] < 0x0f) { // the scope is small enough
    // XXX : SDH : Need to check that the group is small enough
    **buf = LOWPAN_IPHC_HC_MCAST; // set the encoding bits
    **buf |= (addr[1] << LOWPAN_HC_MCAST_SCOPE_OFFSET); // scope

    // direct mapped group id
    **buf |= addr[14] & 0x1;
    *(*buf + 1) = addr[15];
    *buf += 2;
    return LOWPAN_IPHC_ADDR_16;
  } else {
    // fuck it, send the whole thing
    memcpy(*buf, addr, 16);
    *buf += 16;
    return LOWPAN_IPHC_ADDR_128;
  }
}

/*
 * pack the headers of msg into the buffer pointed to by buf.
 * 
 * @returns a pointer to where we stopped writing
 */
uint8_t *packHeaders(ip_msg_t *msg, uint8_t *buf, uint8_t len) {
  uint8_t *dispatch, *encoding, addr_enc;

  dispatch = buf;
  buf += 1;
  encoding = buf;
  buf += 1;
  *encoding = 0;

  if (!(msg->hdr.vlfc[0] == (IPV6_VERSION << 4) && 
        msg->hdr.vlfc[1] == 0 &&
        msg->hdr.vlfc[2] == 0 &&
        msg->hdr.vlfc[3] == 0)) {
    ip_memcpy(buf, &msg->hdr.vlfc, 4);
    buf += 4;
  } else {
    *encoding |= LOWPAN_IPHC_VTF_MASK;
  }

  if (msg->hdr.nxt_hdr == IANA_UDP /* or other compressed values... */) {
    // we will add the HCNH encoding at the end of the header
    *encoding |= LOWPAN_IPHC_NH_MASK;
  } else {
    *buf = msg->hdr.nxt_hdr;
    buf += 1;
  }

  // always carry hop limit
  *buf = msg->hdr.hlim;
  buf += 1;

  if (globalPrefix && cmpPfx(msg->hdr.src_addr, my_address)) {
    *dispatch = LOWPAN_HC_CRP_PATTERN;
  } else if (globalPrefix && cmpPfx(msg->hdr.dst_addr, my_address)) {
    *dispatch = LOWPAN_HC_CRP_PATTERN;
  } else if (cmpPfx(msg->hdr.src_addr, linklocal_prefix)) {
    *dispatch = LOWPAN_HC_LOCAL_PATTERN;
  } else {
    *dispatch = LOWPAN_HC_LOCAL_PATTERN;
  }

  addr_enc = packAddress(*dispatch, &buf, msg->hdr.src_addr);
  *encoding |= addr_enc << LOWPAN_IPHC_SC_OFFSET;

  addr_enc = packAddress(*dispatch, &buf, msg->hdr.dst_addr);
  *encoding |= addr_enc << LOWPAN_IPHC_DST_OFFSET;


  // now come the compressions for special next header values.

  if (msg->hdr.nxt_hdr == IANA_UDP) {
    struct udp_hdr *udp = (struct udp_hdr *)msg->data;
  
    uint8_t *udp_enc = buf;
    uint8_t *cmpr_port = NULL;
    // do the LOWPAN_UDP coding
    *udp_enc = LOWPAN_UDP_DISPATCH;;
    buf += 1;


    if ((ntoh16(udp->srcport) & LOWPAN_UDP_PORT_BASE_MASK) == 
        LOWPAN_UDP_PORT_BASE) {
      //printf("compr to 4b\n");
      cmpr_port = buf;
      *cmpr_port = (ntoh16(udp->srcport) & ~LOWPAN_UDP_PORT_BASE_MASK) << 4;
      *udp_enc |= LOWPAN_UDP_S_MASK;
      buf += 1;
    } else {
      ip_memcpy(buf, (uint8_t *)&udp->srcport, 2);
      buf += 2;
    }

    if ((ntoh16(udp->dstport) & LOWPAN_UDP_PORT_BASE_MASK) == 
        LOWPAN_UDP_PORT_BASE) {
      if (cmpr_port == NULL) {
        // the source port must not have been compressed, so 
        // allocate a new byte for this guy
        *buf = ((ntoh16(udp->dstport) & ~LOWPAN_UDP_PORT_BASE_MASK) << 4);
        buf += 1;
      } else {
        // already in the middle of a byte for the port compression,
        // so fill in the rest of the byte
        *cmpr_port = *cmpr_port | ((ntoh16(udp->dstport) & ~LOWPAN_UDP_PORT_BASE_MASK));
      }
      *udp_enc |= LOWPAN_UDP_D_MASK;
    } else {
      ip_memcpy(buf, (uint8_t *)&udp->dstport, 2);
      buf += 2;
    }
        
    // we never elide the checksum
    ip_memcpy(buf, (uint8_t *)&udp->chksum, 2);
    buf += 2;

  }

  // I think we're done here...
  return buf;
}

/*
 * indicates how much of the packet after the IP header we will pack
 *
 */
int packs_header(ip_msg_t *msg) {
  switch (msg->hdr.nxt_hdr) {
  case IANA_UDP:
    return sizeof(struct udp_hdr);
  default:
    return 0;
  }
}


#define CHAR_VAL(X)  (((X) >= '0' && (X) <= '9') ? ((X) - '0') : ((X) - 'A'))


void inet6_aton(char *addr, ip6_addr_t dest) {
  uint16_t cur = 0;
  char *p = addr;
  uint8_t block = 0, shift = 0;
  if (addr == NULL || dest == NULL) return;
  ip_memclr(dest, 16);

  // go to upper case
  while (*p != '\0') {
    if (*p >= 'a' && *p <= 'z')
      *p -= 'a' - 'A' - 10;
    p ++;
  }
  p = addr;

  // first fill in from the front
  while (*p != '\0') {
    if (*p != ':') {
      cur <<= 4;
      cur |= CHAR_VAL(*p);
    } else {
      ((uint16_t *)dest)[block++] = hton16(cur);
      cur = 0;
    }
    p++;
    if (*p == '\0')
      return;
    if (*(p - 1) == ':' && *p == ':') {
      break;
    }
  }
  // we must have hit a "::" which means we need to start filling in from the end.
  block = 7;
  cur = 0;
  while (*p != '\0') p++;
  p--;
  // now pointing at the end of the address string
  while (p > addr) {
    if (*p != ':') {
      cur |= (CHAR_VAL(*p) << shift);
      shift += 4;
    } else {
      ((uint16_t *)dest)[block--] = hton16(cur);
      cur = 0; shift = 0;
    }
    p --;
    if (*(p + 1) == ':' && *p == ':') break;
  }
}