package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.ArrayList;
import java.util.List;

import static hk.edu.polyu.comp.comp2021.cvfs.model.VirtualDisk.names;

abstract class File {
    protected String name;
    //Names contain all names that have been created for a file.

    public File(String name) {
        if(Valid.isValidName(name)){
            setName(name);
        }
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        names.add(name);
    }

    public abstract int getSize();


}

