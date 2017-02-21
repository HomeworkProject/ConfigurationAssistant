package de.homeworkproject.configassist.generate;

import de.mlessmann.common.L4YGRandom;
import de.mlessmann.config.ConfigNode;
import de.mlessmann.config.except.RootMustStayHubException;

import java.util.function.Consumer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Life4YourGames on 17.02.17.
 */
public class GroupUserGenerator extends Thread {

    private char[] subClassChars;
    private int fromClass;
    private int toClass;
    private UserTemplate[] users;
    private String gNameMask;
    private String passMask;

    private ConfigNode config;

    public GroupUserGenerator(char[] subClassChars, int fromClass, int toClass, ConfigNode groupConfig, UserTemplate[] users, String gNameMask, String passMask) {
        this.subClassChars = subClassChars;
        this.fromClass = fromClass;
        this.toClass = toClass;
        this.config = groupConfig;
        this.users = users;
        this.gNameMask = gNameMask;
        this.passMask = passMask;
    }

    @Override
    public void run() {

        L4YGRandom.initRndIfNotAlready();
        gNameMask = gNameMask.trim();
        if (gNameMask.isEmpty()) gNameMask = "%i%%a%";

        for (int i = fromClass; i<=toClass; i++) {
            for (char subChar : subClassChars) {
                String gName = gNameMask;
                gName = gName.replaceAll("(%i%)", Integer.valueOf(i).toString());
                gName = gName.replaceAll("(%a%)", Character.valueOf(subChar).toString());

                String pass = passMask;
                pass = pass.replaceAll("(%i%)", Integer.valueOf(i).toString());
                pass = pass.replaceAll("(%a%)", Character.valueOf(subChar).toString());
                pass = pass.replaceAll("(%g%)", gName);

                for (UserTemplate user : users) {
                    if (user==null) continue;
                    String name = user.getName();
                    name = name.trim();
                    if (name.isEmpty()) continue;
                    pass = pass.replaceAll("(%u%)", user.getName());

                    Pattern p = Pattern.compile("(%r%)[0-9]+");
                    Matcher m = p.matcher(pass);
                    while (m.matches()) {
                        String grp = m.group();
                        grp = grp.substring(3);
                        Integer count = Integer.parseInt(grp);
                        String r = L4YGRandom.genRandomAlphaNumString(count);
                        pass = pass.replaceFirst("(%r%)[0-9]+", r);
                        m = p.matcher(pass);
                    }

                    //Clone the node
                    ConfigNode uNode = user.getNode().clone();
                    try {
                        config.getNode(gName, "users", name).setValue(uNode.getValue());
                    } catch (RootMustStayHubException e) {
                        throw new RuntimeException("Unexpected RMSHException!", e);
                    }
                    uNode = config.getNode(gName, "users", name);
                    uNode.getNode("onLoad", "passwd", "password").setString(pass);
                    uNode.getNode("onLoad", "passwd", "method").setString(uNode.getNode("onLoad", "passwd", "method").optString("default"));
                }
            }
        }

        if (onDoneConsumer!=null) onDoneConsumer.accept(this);
    }

    private Consumer<GroupUserGenerator> onDoneConsumer;

    public void onDone(Consumer<GroupUserGenerator> c) {
        onDoneConsumer = c;
    }
}
