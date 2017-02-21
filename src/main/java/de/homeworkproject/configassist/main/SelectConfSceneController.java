package de.homeworkproject.configassist.main;

import de.homeworkproject.configassist.confsearch.ConfSearch;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.Parent;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.ProgressBar;
import javafx.scene.input.*;
import javafx.scene.layout.AnchorPane;

import java.io.File;

public class SelectConfSceneController {
    @FXML
    public ProgressBar SelectConfProgress;
    @FXML
    public MenuBar SelectConfMenuBar;
    @FXML
    public Parent SelectConfBody;
    @FXML
    public Menu SelectConfLoadMenu;
    @FXML
    public Menu DetectedConfs;
    public MenuItem NewConfigTrigger;

    @FXML
    public void onLoadPressed(ActionEvent actionEvent) {
        if (!SelectConfProgress.isVisible()) {
            SelectConfProgress.setVisible(true);
            ConfSearch search = new ConfSearch();
            search.onDone(s -> {
                for (int i = DetectedConfs.getItems().size() - 1; i >= 0; i--) {
                    DetectedConfs.getItems().remove(i);
                }
                int[] quick = new int[]{0};
                s.getResults().forEach(r -> {
                    MenuItem item = new MenuItem();
                    item.setId("confSearchResult");
                    item.setText(r);
                    item.setOnAction(this::onSelectConfFile);
                    if (quick[0] < 10) {
                        KeyCombination acc = new KeyCharacterCombination(""+quick[0]++,
                                KeyCombination.ModifierValue.UP,
                                KeyCombination.ModifierValue.DOWN,
                                KeyCombination.ModifierValue.DOWN,
                                KeyCombination.ModifierValue.UP,
                                KeyCombination.ModifierValue.UP);
                        item.setAccelerator(acc);
                    }
                    DetectedConfs.getItems().add(item);
                });
                System.out.println("Found " + s.getResults().size() + " config(s)");
                if (!DetectedConfs.isVisible()) DetectedConfs.setVisible(true);
                if (DetectedConfs.getItems().size()==0) {
                    MenuItem empty = new MenuItem();
                    empty.setText("--none--");
                    DetectedConfs.getItems().add(empty);
                }
                SelectConfProgress.setVisible(false);
            });
            search.start();
        }
    }

    @FXML
    public void onSelectConfFile(ActionEvent actionEvent) {
        System.out.println(actionEvent.getSource().getClass().getSimpleName());
        if (!(actionEvent.getSource() instanceof MenuItem)) return;
        MenuItem item = ((MenuItem) actionEvent.getSource());
        System.out.println("Selecting config:" + item.getText());
        File f = new File(item == NewConfigTrigger ? "newConfig.json" : item.getText());
        if ((f.isFile() && f.canRead() && f.canWrite()) || !f.exists()) {
            Main.getApp().loadConf(f.getAbsoluteFile());
        } else {
            //TODO: Show error message
        }
    }

    @FXML
    public void onClose() {
        Main.getApp().close();
    }
}
