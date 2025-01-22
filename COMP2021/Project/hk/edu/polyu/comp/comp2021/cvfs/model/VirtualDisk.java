package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.ArrayList;
import java.util.List;

public class VirtualDisk {
    private final int diskSize;
    private int remainSize;
    private Directory rootDirectory;
    private Directory workingDirectory;
    public static List<String> names= new ArrayList<>();



    public VirtualDisk(int diskSize) {
        this.diskSize = diskSize;
        this.remainSize = diskSize;
        this.rootDirectory = new Directory("root");
        this.workingDirectory= rootDirectory;
        names = new ArrayList<>();
        rootDirectory.setParentDirectory(null);
        updateRemainSize(40);
    }

    public Directory getWorkingDirectory() {
        return workingDirectory;
    }
    public void setWorkingDirectory(Directory workingDirectory) {
        this.workingDirectory = workingDirectory;
    }
    public int getRemainSize() {
        return remainSize;
    }
    public void updateRemainSize(int Size){
        this.remainSize -= Size;
    }
    public int getDiskSize() {
        return diskSize;
    }

    public Directory getRootDirectory() {
        return rootDirectory;
    }
}
