package de.homeworkproject.configassist.generate;

import de.mlessmann.config.ConfigNode;
import de.mlessmann.config.JSONConfigLoader;

/**
 * Created by Life4YourGames on 17.02.17.
 */
public class UserTemplate {

    private String name;
    private ConfigNode node;

    public UserTemplate(ConfigNode node, String name) {
        this.node = node;
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public ConfigNode getNode() {
        return node;
    }
}
