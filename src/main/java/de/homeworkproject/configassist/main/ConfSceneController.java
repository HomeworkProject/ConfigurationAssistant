package de.homeworkproject.configassist.main;

import de.mlessmann.config.ConfigNode;
import de.mlessmann.config.JSONConfigLoader;
import de.mlessmann.config.api.ConfigLoader;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.Menu;
import javafx.scene.control.TextField;

import java.io.File;

/**
 * Created by Life4YourGames on 08.02.17.
 */
public class ConfSceneController {

    private static ConfSceneController controller;

    public static ConfSceneController getController() {
        return controller;
    }

    //// === === === === ////

    @FXML
    public Menu MenuFile;

    @FXML
    public Label LabelTCPPlainPort;
    @FXML
    public Label LabelTCPSSLPort;
    @FXML
    public Label LabelTCPFTPort;

    @FXML
    public TextField HWCleanupInterval;

    @FXML
    public CheckBox CheckBoxAutoUpdate;
    @FXML
    public CheckBox CheckBoxAutoUpgrade;
    @FXML
    public TextField TextFieldUpdateCheckInterval;

    private ConfigLoader loader;
    private ConfigNode config;
    private ConfigLoader groupLoader;
    private ConfigNode groupConfig;

    public ConfSceneController() {
        controller = this;
    }

    public void loadConfig(ConfigLoader loader) {
        CheckBoxAutoUpdate.requestFocus();

        this.loader = loader;
        this.config = loader.load();

        LabelTCPPlainPort.setText("TCP-Plaintext port: " + config.getNode("tcp", "plain", "port").optInt(11900));
        LabelTCPSSLPort.setText("TCP-SSL port: " + config.getNode("tcp", "ssl", "port").optInt(11901));
        LabelTCPFTPort.setText("TCP-FT port: " + config.getNode("tcp", "ft", "port").optInt(11902));

        HWCleanupInterval.setText("" + config.getNode("cleanup", "hw_database", "maxAgeDays").optInt(60));

        CheckBoxAutoUpdate.setSelected(config.getNode("update", "enable").optBoolean(true));
        CheckBoxAutoUpgrade.setSelected(config.getNode("update", "autoUpgrade").optBoolean(true));
        TextFieldUpdateCheckInterval.setText("" +   config.getNode("update", "interval").optInt(1));

        if (!config.getNode("groups").isVirtual() && config.getNode("groups").isType(String.class)) {
            System.out.println("External group config detected: Loading...");
            File gConf = new File(config.getNode("groups").getString());
            //We need to be able to read and write an actual file - If nonexistent we assume we can
            if ((!gConf.isFile() || !gConf.canRead() || !gConf.canWrite()) && gConf.exists()) {
                onClose(null);
                return;
            }
            groupLoader = new JSONConfigLoader();
            groupConfig = groupLoader.loadFromFile(gConf);
            if (groupLoader.hasError()) {
                //TODO: Error handling
                groupLoader.getError().printStackTrace();
                groupLoader = null;
                groupConfig = null;
                onClose(null);
                return;
            }
        } else {
            groupConfig = config.getNode("groups");
        }
    }

    @FXML
    public void onSetupNet(ActionEvent event) {
        Main.getApp().setupNet();
        SetupNetController c = SetupNetController.getController();
        if (c!=null) c.setConfig(config);
    }

    @FXML
    public void onSetupGroups(ActionEvent event) {
        Main.getApp().setupGroups();
        SetupGroupsController c = SetupGroupsController.getController();
        if (c!=null) c.setConfig(groupConfig);
    }

    @FXML
    public void onSave(ActionEvent event) {

        //Call other controllers to write their info on the config
        try {
            config.getNode("update", "intervalTimeUnit").setString("HOURS");
            config.getNode("update", "interval").setInt(Integer.parseInt(TextFieldUpdateCheckInterval.getText()));
            config.getNode("update", "enable").setBoolean(CheckBoxAutoUpdate.isSelected());
            config.getNode("update", "autoUpgrade").setBoolean(CheckBoxAutoUpgrade.isSelected());

            SetupNetController c = SetupNetController.getController();
            if (c != null) c.save();
        } catch (Exception e) {
            //TODO: Error handling
            System.out.println("Cannot save config - There're errors in your entries: " + e.getMessage());
            e.printStackTrace();
            return;
        }

        loader.resetError();
        loader.save(config);
        if (loader.hasError()) {
            //TODO: Error handling
            loader.getError().printStackTrace();
        }
    }

    @FXML
    public void onClose(ActionEvent event) {
        Main.getApp().loadConf(null);
    }

    public void backToConf() {
        Main.getApp().toConf();
    }
}
