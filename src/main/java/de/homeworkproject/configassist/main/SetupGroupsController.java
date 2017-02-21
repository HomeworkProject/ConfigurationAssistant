package de.homeworkproject.configassist.main;

import com.sun.javafx.collections.ObservableListWrapper;
import de.homeworkproject.configassist.generate.GroupUserGenerator;
import de.homeworkproject.configassist.generate.UserTemplate;
import de.mlessmann.config.ConfigNode;
import javafx.application.Platform;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;

import java.util.*;

/**
 * Created by Life4YourGames on 10.02.17.
 */
public class SetupGroupsController {

    private static SetupGroupsController controller;

    public static SetupGroupsController getController() {
        return controller;
    }

    //// === === === === === === === === === === ////

    private Background invalidBackground = new Background(new BackgroundFill(Color.RED, CornerRadii.EMPTY, Insets.EMPTY));
    private Background validBackground = null;

    @FXML
    public ListView<String> groupList;
    @FXML
    public ListView<String> userList;
    @FXML
    public TextField addGroupField;
    @FXML
    public TextField addUserField;
    @FXML
    public TextField selectedGroup;
    @FXML
    public TextField selectedUser;

    //User generation templates
    @FXML
    public VBox userTemplates;
    private TextField lastTemplate;
    private Map<TextField, ConfigNode> userTemplatePerms;

    //User(-template) config
    private ConfigNode currentUserNode;
    private HBox lastPermHBox;
    private Map<String, String> permNameDesc;
    @FXML
    public GridPane userSetupPane;
    @FXML
    public Label userSetupCaption;
    @FXML
    public Label userSetupGroup;
    @FXML
    public TextField userSetupAuthMethod;
    @FXML
    public TextField userSetupAuthPass;
    @FXML
    public VBox userSetupPermList;

    //User/Group generation
    @FXML
    public ComboBox fromSubClass;
    @FXML
    public ComboBox toSubClass;
    @FXML
    public TextField fromClass;
    @FXML
    public TextField toClass;
    @FXML
    public TextField groupNameMask;
    @FXML
    public TextField passMask;

    private ConfigNode config;

    public SetupGroupsController() {
        controller = this;
        userTemplatePerms = new HashMap<TextField, ConfigNode>();
        permNameDesc = new HashMap<String, String>();
        permNameDesc.put("hw_add", "Allows adding homework");
        permNameDesc.put("hw_edit", "Allows editing homework");
        permNameDesc.put("hw_del", "Allows deleting homework");
    }

    public void initialize() {
        groupList.getSelectionModel().setSelectionMode(SelectionMode.SINGLE);
        groupList.getSelectionModel().selectedItemProperty().addListener((obs, oVal, nVal) -> {
            onSelectGroup(nVal);
        });
        userList.getSelectionModel().setSelectionMode(SelectionMode.SINGLE);
        userList.getSelectionModel().selectedItemProperty().addListener((obs, oVal, nVal) -> {
            onSelectUser(nVal);
        });
    }

    @FXML
    public void onBack() {
        Main.getApp().toConf();
    }

    @FXML
    public void onRemoveUserTemplate(ActionEvent event) {
        ObservableList<Node> templates = userTemplates.getChildren();
        if (lastTemplate!=null) {
            userTemplatePerms.remove(lastTemplate);
            templates.remove(lastTemplate);
            lastTemplate = null;
        } else if (templates.size()>0) {
            //noinspection SuspiciousMethodCalls -> we only put TextFields into the template list
            userTemplatePerms.remove(templates.get(templates.size()-1));
            templates.remove(templates.size()-1);
        }
        if (templates.size() > 0) templates.get(templates.size()-1).requestFocus();
        else loadUserSetting(null, false);
    }

    @FXML
    public void onAddUserTemplate(ActionEvent event) {
        if (validBackground==null) validBackground = addUserField.getBackground();
        ObservableList<Node> templates = userTemplates.getChildren();
        TextField newText = new TextField();
        newText.setBackground(invalidBackground);
        newText.setOnMouseClicked(this::onUserTemplateClicked);
        newText.textProperty().addListener((obs, oldV, newV) -> {
            onUserTemplateTextChange(obs, oldV, newV, newText);
        });
        newText.textProperty().addListener((obs, oVal, nVal) -> {
            if (nVal.isEmpty() || nVal.trim().isEmpty()) {
                newText.setBackground(invalidBackground);
            } else if (newText.getBackground() == invalidBackground) newText.setBackground(validBackground);
        });
        newText.focusedProperty().addListener((obs, oldV, newV) -> {
            onUserTemplateFocused(obs, oldV, newV, newText);
        });
        newText.setOnKeyPressed(e -> {
            if (e.getCode()==KeyCode.ENTER) onAddUserTemplate(null);
            else if (e.getCode()==KeyCode.BACK_SPACE && newText.getText().length()==0) onRemoveUserTemplate(null);
        });
        userTemplatePerms.put(newText, new ConfigNode());
        templates.add(newText);
        newText.requestFocus();
        //lastTemplate = newText;
        //LoadUserSetting not needed as it's called by the focus change above
    }

    @FXML
    public void onUserTemplateClicked(Event event) {
        /*if (event.getSource() instanceof TextField) {
            lastTemplate = (TextField) event.getSource();
            loadUserSetting(userTemplatePerms.get(lastTemplate), true);
        }*/
    }

    public void onUserTemplateFocused(ObservableValue<? extends Boolean> obs, boolean oldValue, boolean newValue, TextField subject) {
        if (newValue) {
            lastTemplate = subject;
            loadUserSetting(userTemplatePerms.get(lastTemplate), true);
        }
    }

    public void onUserTemplateTextChange(Object obs, String oldValue, String newValue, TextField subject) {
        if (subject == lastTemplate)
            userSetupCaption.setText("Editing user: " + lastTemplate.getText());
        else
            System.out.println("WARN User is editing userTemplate while lastTemplate hasn't been modified!");
    }

    @FXML
    public void onStartGenerate(ActionEvent event) {

        Main.getApp().setGenGroupScene(true);

        int fromC = 0;
        int toC = 0;
        char[] subClasses = null;
        UserTemplate[] templates;
        try {
            fromC = Integer.parseInt(fromClass.getText());
            toC = Integer.parseInt(toClass.getText());

            int fromIndex = fromSubClass.getSelectionModel().getSelectedIndex();
            int toIndex = toSubClass.getSelectionModel().getSelectedIndex();

            subClasses = new char[(toIndex - fromIndex) + 1];
            for (int i = fromIndex; i<=toIndex; i++) {
                subClasses[i-fromIndex] = ((String) fromSubClass.getItems().get(i)).charAt(0);
            }

            templates = new UserTemplate[userTemplates.getChildren().size()];
            for (int i = 0; i < userTemplates.getChildren().size(); i++) {
                Node n = userTemplates.getChildren().get(i);
                if (n instanceof TextField) {
                    ConfigNode node = userTemplatePerms.getOrDefault(n, null);
                    if (node != null) {
                        templates[i] = new UserTemplate(node, ((TextField) n).getText());
                    }
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
            Main.getApp().setGenGroupScene(false);
            return;
        }
        GroupUserGenerator gen = new GroupUserGenerator(subClasses, fromC, toC, config, templates, groupNameMask.getText(), passMask.getText());
        gen.onDone(c -> {
            Platform.runLater(() -> {
                Main.getApp().setGenGroupScene(false);
                SetupGroupsController.getController().setConfig(config);
            });
        });
        gen.start();
    }

    public void loadUserSetting(ConfigNode node, boolean useTemplate) {
        if (node == null) {
            currentUserNode = null;
            userSetupPane.setVisible(false);
            return;
        }
        if (useTemplate && lastTemplate==null) {
            loadUserSetting(null, false);
            return;
        }
        if (useTemplate) {
            groupList.getSelectionModel().select(null);
            userList.getSelectionModel().select(null);
            userSetupCaption.setText("Editing user: " + lastTemplate.getText());
            userSetupGroup.setText("Group: <TEMPLATE>");
            userSetupAuthPass.setDisable(true);
        } else {
            userSetupCaption.setText("Editing user: " + selectedUser.getText());
            userSetupGroup.setText("Group: " + selectedGroup.getText());
            userSetupAuthPass.setDisable(false);
        }
        currentUserNode = node;
        String authMethod = currentUserNode.getNode("onLoad", "passwd", "method").optString(null);
        if (authMethod == null) authMethod = currentUserNode.getNode("auth", "method").optString("default");
        userSetupAuthMethod.setText(authMethod);

        String pass = currentUserNode.getNode("onLoad", "passwd", "password").optString(null);
        if (pass == null) pass = currentUserNode.getNode("auth", "pass").optString("default");
        userSetupAuthPass.setText(pass);

        ConfigNode perms = node.getNode("permissions");
        List<String> keys;
        Optional<List<String>> optKeys = perms.getKeys();
        keys = optKeys.orElseGet(ArrayList::new);

        //Generate FX elements for the permission list
        userSetupPermList.getChildren().clear();
        keys.forEach(k -> {
            HBox permBox = new HBox();
            permBox.setSpacing(2);

            TextField pName = new TextField();
            pName.setText(k);
            pName.setTooltip(new Tooltip(permNameDesc.getOrDefault(k, "No help available")));
            pName.textProperty().addListener((obs, oVal, nVal) -> {
                pName.setTooltip(new Tooltip(permNameDesc.getOrDefault(nVal, "No help available")));
            });
            pName.focusedProperty().addListener((obs, oVal, nVal) -> {
                onPermHBoxFocused(permBox, nVal);
            });
            permBox.getChildren().add(pName);

            Label hasLabel = new Label();
            hasLabel.setText("Has:");
            permBox.getChildren().add(hasLabel);

            List<Integer> values = perms.getNode(k, "values").getList();
            TextField hasValue = new TextField();
            hasValue.setPrefWidth(50);
            hasValue.setText(values.get(0) + "");
            permBox.getChildren().add(hasValue);

            userSetupPermList.getChildren().add(permBox);
        });

        userSetupPane.setVisible(true);
    }

    public void onPermHBoxFocused(HBox sender, boolean focused) {
        if (focused) lastPermHBox = sender;
        else if (sender == lastPermHBox) lastPermHBox = null;
    }

    @FXML
    public void onAddPermHBox(ActionEvent event) {
        HBox permBox = new HBox();
        permBox.setSpacing(2);

        TextField pName = new TextField();
        pName.textProperty().addListener((obs, oVal, nVal) -> {
            pName.setTooltip(new Tooltip(permNameDesc.getOrDefault(nVal, "No help available")));
        });
        pName.focusedProperty().addListener((obs, oVal, nVal) -> {
            onPermHBoxFocused(permBox, nVal);
        });
        permBox.getChildren().add(pName);

        Label hasLabel = new Label();
        hasLabel.setText("Has:");
        permBox.getChildren().add(hasLabel);

        TextField hasValue = new TextField();
        hasValue.setPrefWidth(50);
        hasValue.setText("0");
        permBox.getChildren().add(hasValue);
        userSetupPermList.getChildren().add(permBox);
        pName.requestFocus();
    }

    @FXML
    public void onDelPermHBox(ActionEvent event) {
        if (lastPermHBox!=null) userSetupPermList.getChildren().remove(lastPermHBox);
        else if (userSetupPermList.getChildren().size()>0)
            userSetupPermList.getChildren().remove(userSetupPermList.getChildren().size()-1);
    }

    @FXML
    public void onGenDefaultUserPerms(ActionEvent event) {
        if (!userSetupPane.isVisible()) return;
        userSetupPermList.getChildren().clear();

        //HW_ADD
        int[] vals = new int[]{1, 1, 0};
        List<Integer> defVals = new ArrayList<>(3);
        Arrays.stream(vals).forEach(i -> defVals.add(i));
        currentUserNode.getNode("permissions", "hw_add", "values").setList(defVals);

        //HW_EDIT
        currentUserNode.getNode("permissions", "hw_edit", "values").setList(defVals);

        //HW_DEL
        currentUserNode.getNode("permissions", "hw_del", "values").setList(defVals);

        loadUserSetting(currentUserNode, currentUserNode.getKey()==null);
    }

    @FXML
    public void onApplyUserChanges(ActionEvent event) {

        if (currentUserNode==null) return;

        if (!userSetupAuthMethod.getText().equals(currentUserNode.getNode("auth", "method").optString("")))
            currentUserNode.getNode("onLoad", "passwd", "method").setString(userSetupAuthMethod.getText());
        if (!userSetupAuthPass.getText().equals(currentUserNode.getNode("auth", "pass").optString("")))
            currentUserNode.getNode("onLoad", "passwd", "password").setString(userSetupAuthPass.getText());

        //Permissions
        currentUserNode.delNode("permissions");
        userSetupPermList.getChildren().forEach(e -> {
            if (e instanceof HBox) {
                String name = null;
                List<Integer> values = new ArrayList<Integer>();
                if (((HBox) e).getChildren().size()<=0 || !(((HBox) e).getChildren().get(0) instanceof TextField)) return;

                for (Node node : ((HBox) e).getChildren()) {
                    if (node instanceof TextField && name == null)
                        name = ((TextField) node).getText();
                    else if (node instanceof TextField) {
                        try {
                            Integer i = Integer.parseInt(((TextField) node).getText());
                            values.add(i);
                        } catch (NumberFormatException nfe) {
                            values.add(0);
                            //TODO: Better handling ?
                        }
                    }
                }
                if (name!=null && !name.isEmpty() && !name.trim().isEmpty() && values.size()>0)
                    currentUserNode.getNode("permissions", name, "values").setList(values);
            }
        });
    }

    @FXML
    public Label groupUserCount;

    public void setConfig(ConfigNode config) {
        this.config = config;
        groupList.getItems().clear();
        userList.getItems().clear();

        Optional<List<String>> optGroups = config.getKeys();
        if (optGroups.isPresent()) {
            List<String> groups = optGroups.get();
            groups.sort(String::compareTo);

            groupList.setItems(new ObservableListWrapper<String>(groups));

            int gCount = groupList.getItems().size();
            int[] uCount = new int[]{0};
            groups.forEach(g -> {
                uCount[0] = uCount[0] + config.getNode(g, "users").getKeys().map(List::size).orElse(0);
            });
            groupUserCount.setText(uCount[0] + " users in \n" + gCount + " groups");
        }

        onSelectGroup(null);
    }

    public void onSelectGroup(String group) {
        selectedGroup.setText("");
        userList.getItems().clear();

        if (group!=null) {
            selectedGroup.setText(group);
            Optional<List<String>> optUsers = config.getNode(group, "users").getKeys();
            if (optUsers.isPresent()) {
                List<String> users = optUsers.get();
                userList.setItems(new ObservableListWrapper<String>(users));
                onSelectUser(null);
            }
        }
    }

    @FXML
    public void onSelectUser(String user) {
        if (user!=null) {
            selectedUser.setText(user);
            loadUserSetting(config.getNode(selectedGroup.getText(), "users", user), false);
        } else {
            selectedUser.setText("");
            loadUserSetting(null, false);
        }
    }

    @FXML
    public void onCreateGroup(ActionEvent event) {
        String groupName = addGroupField.getText();
        groupName = groupName.trim();
        if (groupName.isEmpty()) {
            //TODO: Better feedback
            return;
        }
        config.getNode(groupName, "name").setString(groupName);
        addGroupField.setText("");
        setConfig(config);
        groupList.getSelectionModel().select(groupName);
    }

    @FXML
    public void onCreateUser(ActionEvent event) {
        String groupName = groupList.getSelectionModel().getSelectedItem();
        if (groupName == null || groupName. trim().isEmpty()) {
            //TODO: Better feedback
            return;
        }
        groupName = groupName.trim();
        String userName = addUserField.getText();
        userName = userName.trim();
        if (userName.isEmpty()) {
            //TODO: Better feedback
            return;
        }
        config.getNode(groupName, "users", userName, "name").setString(userName);
        addUserField.setText("");
        setConfig(config);
        groupList.getSelectionModel().select(groupName);
        userList.getSelectionModel().select(userName);
    }

    @FXML
    public void onRemoveGroup(ActionEvent event) {
        String groupName = selectedGroup.getText();
        groupName = groupName.trim();
        if (groupName.isEmpty()) {
            //TODO: Better feedback
            return;
        }
        config.delNode(groupName);
        setConfig(config);
    }

    @FXML
    public void onRemoveUser(ActionEvent event) {
        String groupName = groupList.getSelectionModel().getSelectedItem();
        if (groupName == null || groupName.trim().isEmpty()) {
            //TODO: Better feedback
            return;
        }
        groupName = groupName.trim();
        String userName = selectedUser.getText();
        userName = userName.trim();
        if (userName.isEmpty()) {
            //TODO: Better feedback
            return;
        }
        config.getNode(groupName, "users").delNode(userName);
        setConfig(config);
        groupList.getSelectionModel().select(groupName);
    }

    @FXML
    public void onDelGroups(ActionEvent event) {
        Optional<List<String>> optGroups = config.getKeys();
        optGroups.ifPresent(strings -> strings.forEach(config::delNode));
        setConfig(config);
    }
}
