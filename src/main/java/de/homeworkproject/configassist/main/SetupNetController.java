package de.homeworkproject.configassist.main;

import de.mlessmann.config.ConfigNode;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextField;
import javafx.stage.FileChooser;

import java.io.File;

/**
 * Created by Life4YourGames on 09.02.17.
 */
public class SetupNetController {

    private static SetupNetController controller;
    public static SetupNetController getController() {
        return controller;
    }

    //// === === === === === === === === === === ////

    private ConfigNode config;

    @FXML
    public CheckBox enablePlainTCP;
    @FXML
    public TextField portPlainTCP;

    @FXML
    public CheckBox enableSSLTCP;
    @FXML
    public TextField portSSLTCP;
    @FXML
    public TextField keystorePass;
    @FXML
    public TextField keystorePath;

    @FXML
    public CheckBox enableFTTCP;
    @FXML
    public TextField portFTTCP;

    public SetupNetController() {
        controller = this;
    }

    @FXML
    public void onBack(ActionEvent event) {
        ConfSceneController.getController().backToConf();
    }

    @FXML
    public void chooseKeystore(ActionEvent event) {
        FileChooser chooser = new FileChooser();
        chooser.setTitle("Select keystore...");
        chooser.setSelectedExtensionFilter(new FileChooser.ExtensionFilter("Default", "ks", "jks", "keystore"));
        File f = chooser.showOpenDialog(null);
        //TODO: Set TextField content
        if (f!=null) keystorePath.setText(f.getAbsoluteFile().getPath());
    }

    public void setConfig(ConfigNode config) {
        this.config = config;

        ConfigNode net = config.getNode("tcp");

        enablePlainTCP.setSelected(net.getNode("plain", "enabled").optBoolean(true));
        portPlainTCP.setText(net.getNode("plain", "port").optInt(11901).toString());

        enableSSLTCP.setSelected(net.getNode("ssl", "enabled").optBoolean(false));
        portSSLTCP.setText(net.getNode("ssl", "port").optInt(11902).toString());
        keystorePass.setText(net.getNode("ssl", "password").optString("MyKeystorePass"));
        keystorePath.setText(net.getNode("ssl", "keystore").optString("keystore.ks"));

        enableFTTCP.setSelected(net.getNode("ft", "enabled").optBoolean(false));
        portFTTCP.setText(net.getNode("ft", "port").optInt(11903).toString());
    }

    public void save() {
        ConfigNode net = config.getNode("tcp");

        try {
            net.getNode("plain", "enabled").setBoolean(enablePlainTCP.isSelected());
            net.getNode("plain", "port").setInt(Integer.valueOf(portPlainTCP.getText()));

            net.getNode("ssl", "enabled").setBoolean(enableSSLTCP.isSelected());
            net.getNode("ssl", "port").setInt(Integer.valueOf(portSSLTCP.getText()));
            net.getNode("ssl", "password").setString(keystorePass.getText());
            net.getNode("ssl", "keystore").setString(keystorePath.getText());

            net.getNode("ft", "enable").setBoolean(enableFTTCP.isSelected());
            net.getNode("ft", "port").setInt(Integer.valueOf(portFTTCP.getText()));
        } catch (NumberFormatException nfe) {
            nfe.printStackTrace();
            //TODO: Error handling
        }
    }
}
