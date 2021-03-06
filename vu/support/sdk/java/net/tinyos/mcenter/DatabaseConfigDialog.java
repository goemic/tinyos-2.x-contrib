/*
 * Copyright (c) 2003, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 * 
 * DatabaseConfigDialog.java
 *
 * Created on August 20, 2003, 12:46 PM
 */

package net.tinyos.mcenter;

import net.tinyos.mcenter.treeTable.*;

import java.util.prefs.*;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.Document;

/**
 * Dialog to provide the functionality of saving the configuration of the applications
 * in the MessageCenter. It Creates a single XML file from the configurations selected
 * on dialog. The dialog is also capable to load a saved XML file completely or only 
 * parts of it    
 * @author  Andras Nadas
 */
public class DatabaseConfigDialog extends javax.swing.JDialog {
    
    protected static boolean standalone = false;
    
    private PreferenceModel localModel;
    private javax.swing.JTable localTable;
    
    private XmlPreferenceModel importModel = null;
    private javax.swing.JTable importTable;
    /** Creates new form DatabaseConfigDialog */
    public DatabaseConfigDialog(java.awt.Frame parent, boolean modal) {
        super(parent, "Database Configuration", modal);
        /*try{
            localModel = new PreferenceModel(Class.forName("isis.nest.messageCenter.CenterFrame"));
        }catch(java.lang.ClassNotFoundException cnfe){
            System.err.println("Class not Found");
            localModel = new PreferenceModel();
        }*/
        
        localModel = new PreferenceModel();
        
        localTable = new JTreeTable(localModel);
        localModel.setTreeTable((JTreeTable)localTable);
        initComponents();
        localTableScrollPane.setViewportView(localTable);
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    private void initComponents() {//GEN-BEGIN:initComponents
        java.awt.GridBagConstraints gridBagConstraints;

        jTabbedPane1 = new javax.swing.JTabbedPane();
        localDataPanel = new javax.swing.JPanel();
        localTableScrollPane = new javax.swing.JScrollPane();
        localControlPanel = new javax.swing.JPanel();
        exportButton = new javax.swing.JButton();
        reloadButton = new javax.swing.JButton();
        importDataPanel = new javax.swing.JPanel();
        importTableScrollPane = new javax.swing.JScrollPane();
        importControlPanel = new javax.swing.JPanel();
        openButton = new javax.swing.JButton();
        importButton = new javax.swing.JButton();
        closeButton = new javax.swing.JButton();

        getContentPane().setLayout(new java.awt.GridBagLayout());

        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                closeDialog(evt);
            }
        });

        localDataPanel.setLayout(new java.awt.GridBagLayout());

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        localDataPanel.add(localTableScrollPane, gridBagConstraints);

        localControlPanel.setLayout(new java.awt.GridBagLayout());

        exportButton.setText("Export Selection ...");
        exportButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                exportButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 24);
        localControlPanel.add(exportButton, gridBagConstraints);

        reloadButton.setText("Reload DB");
        reloadButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                reloadButtonActionPerformed(evt);
            }
        });

        localControlPanel.add(reloadButton, new java.awt.GridBagConstraints());

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weighty = 0.1;
        localDataPanel.add(localControlPanel, gridBagConstraints);

        jTabbedPane1.addTab("Local Database", localDataPanel);

        importDataPanel.setLayout(new java.awt.GridBagLayout());

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        importDataPanel.add(importTableScrollPane, gridBagConstraints);

        importControlPanel.setLayout(new java.awt.GridBagLayout());

        openButton.setText("Open ...");
        openButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 0, 24);
        importControlPanel.add(openButton, gridBagConstraints);

        importButton.setText("Import Selection");
        importButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                importButtonActionPerformed(evt);
            }
        });

        importControlPanel.add(importButton, new java.awt.GridBagConstraints());

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 0.1;
        importDataPanel.add(importControlPanel, gridBagConstraints);

        jTabbedPane1.addTab("Import Database", importDataPanel);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        getContentPane().add(jTabbedPane1, gridBagConstraints);

        closeButton.setText("Close");
        closeButton.setAlignmentY(0.9F);
        closeButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                closeButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
        gridBagConstraints.insets = new java.awt.Insets(6, 0, 6, 5);
        getContentPane().add(closeButton, gridBagConstraints);

        java.awt.Dimension screenSize = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
        setBounds((screenSize.width-400)/2, (screenSize.height-300)/2, 400, 300);
    }//GEN-END:initComponents

    private void reloadButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_reloadButtonActionPerformed
        /*try{
            localModel = new PreferenceModel(Class.forName("isis.nest.messageCenter.CenterFrame"));
        }catch(java.lang.ClassNotFoundException cnfe){
            System.err.println("Class not Found");
            localModel = new PreferenceModel();
        }*/
        localModel = new PreferenceModel();
        localTable = new JTreeTable(localModel);
        localModel.setTreeTable((JTreeTable)localTable);
        localTableScrollPane.setViewportView(localTable);
    }//GEN-LAST:event_reloadButtonActionPerformed
    
    private void importButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_importButtonActionPerformed
        //try{
            if(importModel != null)
                ((XmlPreferenceModel)importModel).importSelection( Preferences.userRoot());
        /*}catch(java.lang.ClassNotFoundException cnfe){
            System.err.println("Class not Found");
        }*/
        
    }//GEN-LAST:event_importButtonActionPerformed
    
    private void openButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openButtonActionPerformed
        javax.swing.JFileChooser chooser = new javax.swing.JFileChooser();
        chooser.setDialogTitle("Open..");
        javax.swing.filechooser.FileFilter filter = new javax.swing.filechooser.FileFilter(){
            public boolean accept(java.io.File f){
                if(f.isDirectory())
                    return true;
                String ext = getExtension(f);
                if(ext != null)
                    return ext.equalsIgnoreCase("xml");
                else
                    return false;
            }
            
            public String getDescription(){
                return ".XML files";
            }
            
        };
        
        chooser.setFileFilter(filter);
        java.io.File selectedFile = null;
        int returnVal = chooser.showOpenDialog(this);
        if(returnVal == javax.swing.JFileChooser.APPROVE_OPTION) {
            
            selectedFile = chooser.getSelectedFile();
            if (selectedFile ==null)
                return;
            
            if(!getExtension(selectedFile).equalsIgnoreCase("xml")){
                System.out.println("Cannot import choosen file: " +selectedFile.getName());
                return;
            }
            try{
                
                SAXBuilder builder = new SAXBuilder();
                Document doc = builder.build(selectedFile);
                // If there are no well-formedness errors,
                // then no exception is thrown
                importModel = new XmlPreferenceModel(doc);
                importTable = new JTreeTable(importModel);
                importModel.setTreeTable((JTreeTable)importTable);
                importTableScrollPane.setViewportView(importTable);
                
            }
            // indicates a well-formedness error
            catch (JDOMException e) {
                System.out.println(selectedFile + " is not well-formed.");
                System.out.println(e.getMessage());
            }catch( java.io.FileNotFoundException fnfe){
                System.err.println("Cannot open file: "+selectedFile.getAbsolutePath());
                
            }catch( java.io.IOException ioe){
                System.err.println("Cannot read file: "+selectedFile.getAbsolutePath());
                System.out.println("Could not check " + selectedFile+ " because " + ioe.getMessage());
            }
            
        }
    }//GEN-LAST:event_openButtonActionPerformed
    
    private void closeButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_closeButtonActionPerformed
        closeDialog(null);
    }//GEN-LAST:event_closeButtonActionPerformed
    
    private void exportButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_exportButtonActionPerformed
        javax.swing.JFileChooser chooser = new javax.swing.JFileChooser();
        chooser.setDialogTitle("Save to..");
        javax.swing.filechooser.FileFilter filter = new javax.swing.filechooser.FileFilter(){
            public boolean accept(java.io.File f){
                if(f.isDirectory())
                    return true;
                String ext = getExtension(f);
                if(ext != null)
                    return ext.equalsIgnoreCase("xml");
                else
                    return false;
            }
            
            public String getDescription(){
                return ".XML files";
            }
            
        };
        
        chooser.setFileFilter(filter);
        java.io.File selectedFile = null;
        int returnVal = chooser.showSaveDialog(this);
        if(returnVal == javax.swing.JFileChooser.APPROVE_OPTION) {
            
            selectedFile = chooser.getSelectedFile();
            if (selectedFile ==null)
                return;
            
            if(!getExtension(selectedFile).equalsIgnoreCase("xml"))
                selectedFile = new java.io.File(selectedFile.getParent(),selectedFile.getName()+".xml");
            
            
            System.out.println("You've choosen to save into this file: " +selectedFile.getName());
            
            
            try{
                localModel.exportSelection(new java.io.FileOutputStream(selectedFile));
            }catch( java.io.FileNotFoundException fnfe){
                System.err.println("Cannot open file: "+selectedFile.getAbsolutePath());
                
            }
        }
    }//GEN-LAST:event_exportButtonActionPerformed
    
    /** Closes the dialog */
    private void closeDialog(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_closeDialog
        if(standalone){
            System.exit(1);
        }else{
            setVisible(false);
            dispose();
        }
    }//GEN-LAST:event_closeDialog
    
    /**
     * Main function for testing.
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        DatabaseConfigDialog dialog = new DatabaseConfigDialog(new javax.swing.JFrame(), true);
        dialog.show();
        DatabaseConfigDialog.standalone = true;
    }
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton closeButton;
    private javax.swing.JButton exportButton;
    private javax.swing.JButton importButton;
    private javax.swing.JPanel importControlPanel;
    private javax.swing.JPanel importDataPanel;
    private javax.swing.JScrollPane importTableScrollPane;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JPanel localControlPanel;
    private javax.swing.JPanel localDataPanel;
    private javax.swing.JScrollPane localTableScrollPane;
    private javax.swing.JButton openButton;
    private javax.swing.JButton reloadButton;
    // End of variables declaration//GEN-END:variables
    
        /*
         * Get the extension of a file.
         */
    private static String getExtension(java.io.File f) {
        String ext = null;
        String s = f.getName();
        int i = s.lastIndexOf('.');
        
        if(i == -1)
            return "";
        
        if (i > 0 &&  i < s.length() - 1) {
            ext = s.substring(i+1).toLowerCase();
        }
        return ext;
    }
    
}
