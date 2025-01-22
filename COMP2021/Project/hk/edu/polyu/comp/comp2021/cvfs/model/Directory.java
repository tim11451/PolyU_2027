package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.List;

public class Directory extends File {
    private List<File> files = new ArrayList<>();
    private List<Directory> subDirectories = new ArrayList<>();
    private Directory parentDirectory = null;



    public Directory(String name) {
        super(name);
    }


    public List<File> getFiles() {
        return files;
    }
    public ArrayList<Document> getDocuments() {
        ArrayList<Document> documents = new ArrayList<>();

        for (File f : getFiles()) {
             if (f instanceof Document) {
                 documents.add((Document) f);
            }
        }


        /*for (Directory subDir : subDirectories) {
            documents.addAll(subDir.getDocuments());
        }*/

        return documents;
    }

    public File getFile(String name) {
        for (int i = 0; i < files.size(); i++) {
            if (files.get(i).getName().equals(name)) {
                return files.get(i);
            }
        }
        return null;
    }
    public boolean existFile(String fileName) {
        if(getFile(fileName) != null){
            return true;
        }
        else{
            System.out.println("File <" + fileName + "> does not exist.");
            return false;
        }
    }


    public void addDoc(Document document) {
        files.add(document);
    }
    public void addDir(Directory directory){
        files.add(directory);
        subDirectories.add(directory);
        directory.setParentDirectory(this);
    }

    public void removeDoc(Document document) {
        files.remove(document);
    }
    public void removeDir(Directory directory){
        files.remove(directory);
        subDirectories.remove(directory);
        directory.setParentDirectory(null);
    }

    public void setSubDirectories(List<Directory> subDirectories) {
        this.subDirectories = subDirectories; //setter of subdirectories, must set before testing
    }
    public List<Directory> getSubDirectories() {
        return subDirectories;
    }

    public boolean existSubDirectory(String dirName) {
        if(getSubDirectory(dirName) != null){
            return true;
        }
        else{
            System.out.println("Directory <" + dirName + ">does not exist.");
            return false;
        }
    }
    public Directory getSubDirectory(String dirName) { //
        for (Directory dir:this.subDirectories) {
            if (dir.getName().equals(dirName)) {
                return dir;
            }
        }
        return null;
    }
    public void addSubDirectory(List<Directory> subDirectories, Directory newsubDirectory) {
        this.subDirectories.add(newsubDirectory);
    }

    public void setParentDirectory(Directory parentDirectory) {
        this.parentDirectory = parentDirectory; //setter of parentdirectory
    }
    public Directory getParentDirectory() {
        return parentDirectory;
    }


    @Override
    public int getSize(){
        int size=40;
        for(int i = 0; i< files.size(); i++){
            size+= files.get(i).getSize();
        }
        return size;
    }
}
