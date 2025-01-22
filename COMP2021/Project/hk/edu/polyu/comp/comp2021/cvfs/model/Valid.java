package hk.edu.polyu.comp.comp2021.cvfs.model;


import static hk.edu.polyu.comp.comp2021.cvfs.model.VirtualDisk.names;

public class Valid {
    public static boolean isValidSizeOperation(String op) {
        return op.equals(">") || op.equals("<") || op.equals(">=") || op.equals("<=") || op.equals("==") || op.equals("!=");
    }

    public static boolean isInteger(String val) {
        try {
            Integer.parseInt(val);
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }

    public static boolean isValidLogicOp(String op){
        return op.equals("&&") || op.equals("||");
    }

    public static boolean isValidName(String name){
        if(name.length()>10){
            return false;
        }
        for(int i=0;i<name.length();i++){
            char c = name.charAt(i);
            if(!((c>='0'&&c<'9')||(c>='A'&&c<'Z')||(c>='a'&&c<'z'))){
                return false;
            }
        }
        for(String n:names){
            if(name.equals(n)){
                return false;
            }
        }
        return true;
    }

    public static boolean isValidType(String type) {
        return type.equals("txt")||type.equals("java")||type.equals("html")||type.equals("css");
    }
}

