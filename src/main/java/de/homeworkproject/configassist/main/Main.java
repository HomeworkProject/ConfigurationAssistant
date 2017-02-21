package de.homeworkproject.configassist.main;

import de.mlessmann.config.ConfigNode;
import de.mlessmann.config.JSONConfigLoader;
import de.mlessmann.config.api.ConfigLoader;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.File;
import java.io.IOException;

public class Main extends Application {

    public static void main(String[] args) {
        launch(args);
    }

    private static Main app;
    public static Main getApp() {
        return app;
    }

    // === === === === //

    private Stage mainStage;

    private Scene selectConfScene;
    private Scene confScene;
    private Scene setupSSLScene;
    private Scene setupGroupsScene;
    private Scene generateGroupsScene;
    private ConfigLoader configLoader;
    private ConfigNode config;

    @Override
    public void start(Stage primaryStage) throws Exception {
        app = this;
        Parent selectConfLayout = FXMLLoader.load(getClass().getClassLoader().getResource("SelectConfScene.fxml"));
        selectConfScene = new Scene(selectConfLayout, 300, 275);

        mainStage = primaryStage;
        primaryStage.setTitle("Homework_Server configuration assistant");
        //primaryStage.setResizable(false);
        primaryStage.setScene(selectConfScene);
        primaryStage.show();
    }

    public void loadConf(File file) {
        if (file==null) {
            mainStage.setScene(selectConfScene);
            confScene = null;
            setupSSLScene = null;
            setupGroupsScene = null;
            generateGroupsScene = null;
            return;
        }
        System.out.println("Loading config: " + file.getAbsoluteFile().getPath());
        if (configLoader == null) configLoader = new JSONConfigLoader();
        configLoader.resetError();
        config = configLoader.loadFromFile(file);
        if (configLoader.hasError()) {
            System.out.println("Loading failed: " + configLoader.getError().getMessage());
            configLoader.getError().printStackTrace();
            //TODO: Proper error message
        } else {
            //TODO: Load configuration scene :)
            Parent confLayout = null;
            try {
                confLayout = FXMLLoader.load(getClass().getClassLoader().getResource("ConfScene.fxml"));
                confScene = new Scene(confLayout, 800, 600);
                confScene.getStylesheets().add(getClass().getClassLoader().getResource("ConfScene.css").toExternalForm());
                confScene.getStylesheets().add(getClass().getClassLoader().getResource("style.css").toExternalForm());
                ConfSceneController.getController().loadConfig(configLoader);
            } catch (IOException e) {
                //TODO: Error handling
                e.printStackTrace();
                return;
            }
            mainStage.setScene(confScene);
        }
    }

    public void setupNet() {
        if (setupSSLScene == null) {
            Parent sslLayout;
            try {
                sslLayout = FXMLLoader.load(getClass().getClassLoader().getResource("SetupNetScene.fxml"));
                setupSSLScene = new Scene(sslLayout, confScene.getWidth(), confScene.getHeight());
                setupSSLScene.getStylesheets().add(getClass().getClassLoader().getResource("style.css").toExternalForm());
            } catch (IOException e) {
                //TODO: Error handling
                e.printStackTrace();
                return;
            }
        }
        mainStage.setScene(setupSSLScene);
    }

    public void setupGroups() {
        if (setupGroupsScene == null) {
            Parent groupLayout;
            try {
                groupLayout = FXMLLoader.load(getClass().getClassLoader().getResource("SetupGroupsScene.fxml"));
                setupGroupsScene = new Scene(groupLayout, confScene.getWidth(), confScene.getHeight());
                setupGroupsScene.getStylesheets().add(getClass().getClassLoader().getResource("style.css").toExternalForm());
                //SetupGroupsController.getController().initialize();
            } catch (IOException e) {
                //TODO: Error handling
                e.printStackTrace();
                return;
            }
        }
        setupGenGroups();
        mainStage.setScene(setupGroupsScene);
    }

    public void setupGenGroups() {
        if (generateGroupsScene==null) {
            Parent genGroupLayout;
            try {
                genGroupLayout = FXMLLoader.load(getClass().getClassLoader().getResource("GenGroupsScene.fxml"));
                generateGroupsScene = new Scene(genGroupLayout, confScene.getWidth(), confScene.getHeight());
                generateGroupsScene.getStylesheets().add(getClass().getClassLoader().getResource("style.css").toExternalForm());
            } catch (IOException e) {
                //TODO: Error handling
                e.printStackTrace();
                return;
            }
        }
    }

    public synchronized void setGenGroupScene(boolean active) {
        if (active)
            mainStage.setScene(generateGroupsScene);
        else
            mainStage.setScene(setupGroupsScene);
    }

    public void toConf() {
        if (confScene!=null) mainStage.setScene(confScene);
    }

    public void close() {
        mainStage.close();
    }
}
