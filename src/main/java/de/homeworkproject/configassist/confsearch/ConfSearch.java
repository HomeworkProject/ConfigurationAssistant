package de.homeworkproject.configassist.confsearch;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.function.Consumer;

/**
 * Created by Life4YourGames on 07.02.17.
 */
public class ConfSearch extends Thread {

    private List<String> results;
    private Consumer<ConfSearch> doneConsumer;

    public ConfSearch() {
        results = new ArrayList<String>();
    }

    @Override
    public void run() {
        //New local directory
        File file = new File(".");
        File[] files = file.listFiles(f -> f.isFile() && f.getName().endsWith(".json") && f.canRead() && f.canWrite());
        if (files == null) {
            if (doneConsumer != null) doneConsumer.accept(this);
            return;
        }
        List<String> res = new ArrayList<String>();
        Arrays.stream(files).forEach(f -> res.add(f.getPath()));
        results = res;
        if (doneConsumer!=null) doneConsumer.accept(this);
    }

    public void onDone(Consumer<ConfSearch> con) {
        doneConsumer = con;
    }

    public List<String> getResults() {
        return results;
    }
}
