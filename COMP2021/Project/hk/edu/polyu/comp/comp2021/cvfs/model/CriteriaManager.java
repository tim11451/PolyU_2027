package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

public class CriteriaManager {
    private Map<String, SimpleCriterion> criteriaMap = new HashMap<>();

    public Map<String, SimpleCriterion> getCriteriaMap() {
        return criteriaMap;
    }

    // req-10
    public CriteriaManager() {
        criteriaMap.put("IsDocument", null);
    }

    public boolean ifCriExist(String criName){
        if(criteriaMap.get(criName) == null){
            return false;
        }
        else return true;
    }
    public SimpleCriterion getCriterionByName(String criName) {
        return criteriaMap.get(criName);
    }

    // req-9
    public void newSimpleCri(String criName, String attrName, String op, String val) {
        if(ifCriExist(criName)){
            System.out.println("This criteria already exists. Please change to another name.");
            return;
        }

        if (criName.length() != 2 || !criName.matches("[a-zA-Z]{2}")) {
            System.out.println("criName must contain exactly two English letters.");
            return;
        }

        if (!attrName.equals("name") && !attrName.equals("type") && !attrName.equals("size")) {
            System.out.println("attrName must be either 'name', 'type', or 'size'.");
            return;
        }


        if (attrName.equals("name")) {
            if (!op.equals("contains")) {
                System.out.println("For 'name', op must be 'contains' and val must be a string in double quotes.");
                return;
            }
        } else if (attrName.equals("type")) {
            if (!op.equals("equals") ) {
                System.out.println("For 'type', op must be 'equals' and val must be a string in double quotes.");
                return;
            }
        } else if (attrName.equals("size")) {
            if (!Valid.isValidSizeOperation(op) || !Valid.isInteger(val)) {
                System.out.println("For 'size', op must be one of >, <, >=, <=, ==, != and val must be an integer.");
                return;
            }
        }

        SimpleCriterion newCriterion = new SimpleCriterion(criName,attrName, op, val);
        criteriaMap.put(criName, newCriterion);
        System.out.println("Criterion <" + criName + "> created successfully.");
    }
    //req-11, first
    public void newNegation(String cri1Name,String cri2Name){
        if(!ifCriExist(cri1Name)){
            System.out.println("This criteria does not exist.");
            return;
        }
        SimpleCriterion cri1 = getCriterionByName(cri1Name);
        if(cri1.getAttrName().equals("name")){
            String newOP = "doesn't contain";
            newSimpleCri(cri2Name,"name",newOP,cri1.getVal());
        }
        if(cri1.getAttrName().equals("type")){
            String newOP = "not equal";
            newSimpleCri(cri2Name,"type",newOP,cri1.getVal());
        }
        if(cri1.getAttrName().equals("size")){
            String oldOP = cri1.getOp();
            switch (oldOP) {
                case ">" -> newSimpleCri(cri2Name, "size", "<=", cri1.getVal());
                case "<=" -> newSimpleCri(cri2Name, "size", ">", cri1.getVal());
                case ">=" -> newSimpleCri(cri2Name, "size", "<", cri1.getVal());
                case "<" -> newSimpleCri(cri2Name, "size", ">=", cri1.getVal());
                case "==" -> newSimpleCri(cri2Name, "size", "!=", cri1.getVal());
                case "!=" -> newSimpleCri(cri2Name, "size", "==", cri1.getVal());
            }
        }
    }
    //req-11, second command
    public void newBinaryCri(String cri1Name,String cri2Name,String logicOp,String cri3Name){

        if(!ifCriExist(cri2Name)||!ifCriExist(cri3Name)){
            System.out.println("A / both criteria not exist.");
            return;
        }
        if(!(logicOp.equals("&&")||logicOp.equals("||"))){
            System.out.println("Wrong logic operator.");
            return;
        }

        SimpleCriterion cri2 = getCriterionByName(cri2Name);
        SimpleCriterion cri3 = getCriterionByName(cri3Name);
        String newAttrName = cri2.getAttrName() + logicOp + cri3.getAttrName();
        String[] newOp = new String[]{cri2.getOp(),cri3.getOp()};
        String[] newVal = new String[]{cri2.getVal(),cri3.getVal()};
        SimpleCriterion newCriterion = new SimpleCriterion(cri1Name, newAttrName, Arrays.toString(newOp), Arrays.toString(newVal));
        criteriaMap.put(cri1Name, newCriterion);
        System.out.println("Criterion <" + cri1Name + "> created successfully.");
    }

    //req-12
    public void PrintAllSimpleCri() {
        System.out.printf("| %-10s | %-15s | %-10s | %-10s |\n", "Criteria Name", "Attribute Name", "Operator", "Value");
        System.out.println("|------------|-----------------|------------|------------|");

        for (Map.Entry<String, SimpleCriterion> entry : getCriteriaMap().entrySet()) {
            if (entry.getValue() != null) {
                System.out.printf("| %-10s | %-15s | %-10s | %-10s |\n", entry.getKey(),
                        entry.getValue().getAttrName(), entry.getValue().getOp(), entry.getValue().getVal());
            } else {
                System.out.printf("| %-10s | %-15s | %-10s | %-10s |\n", entry.getKey(), "N/A", "N/A", "N/A");
            }
        }
    }
    // check if a document fit the criteria
    public boolean fitSimpleCri(Document doc, String criName){
        SimpleCriterion cri = criteriaMap.get(criName);
        if(cri.getAttrName().equals("size")){
            switch (cri.getOp()) {
                case ">" -> {
                    return doc.getSize() > Integer.parseInt(cri.getVal());
                }
                case ">=" -> {
                    return doc.getSize() >= Integer.parseInt(cri.getVal());
                }
                case "<" -> {
                    return doc.getSize() < Integer.parseInt(cri.getVal());
                }
                case "<=" -> {
                    return doc.getSize() <= Integer.parseInt(cri.getVal());
                }
            }
        }
        else if(cri.getAttrName().equals("type")){
            return cri.getVal().equals(doc.getType());
        }
        else if(cri.getAttrName().equals("name")){
            return doc.name.contains(cri.getVal());
        }
        else if (cri.getAttrName().equals("size&&name")){
            String OperatorWithoutBrackets = cri.getOp().substring(1, cri.getOp().length() - 1);
            String[] operator= OperatorWithoutBrackets.split(", "); // > contains
            String Values = cri.getVal().substring(1, cri.getVal().length() - 1);
            String[] values = Values.split(", ");// 100 h
            if(operator[0].equals(">")){return doc.getSize()>Integer.parseInt(values[0]) && doc.name.contains(values[1]); }
            else if (operator[0].equals(">=")) {return doc.getSize()>=Integer.parseInt(values[0]) && doc.name.contains(values[1]);}
            else if(operator[0].equals("<=")) {return doc.getSize()<=Integer.parseInt(values[0]) && doc.name.contains(values[1]);}
            else if (operator[0].equals("<")) {return doc.getSize()<Integer.parseInt(values[0]) && doc.name.contains(values[1]);}
            else if(operator[0].equals("==")){return doc.getSize()==Integer.parseInt(values[0]) && doc.name.contains(values[1]);}
            else if (operator[0].equals("!=")){return doc.getSize()!=Integer.parseInt(values[0]) && doc.name.contains(values[1]);}

        }
        else if(cri.getAttrName().equals("name&&size")){
            String OperatorWithoutBrackets = cri.getOp().substring(1, cri.getOp().length() - 1);
            String[] operator= OperatorWithoutBrackets.split(", "); //contains <
            String Values = cri.getVal().substring(1, cri.getVal().length() - 1);
            String[] values = Values.split(", ");// h 100
            if(operator[1].equals(">")){return doc.getSize()>Integer.parseInt(values[1]) && doc.name.contains(values[0]); }
            else if (operator[1].equals(">=")) {return doc.getSize()>=Integer.parseInt(values[1]) && doc.name.contains(values[0]);}
            else if(operator[1].equals("<=")) {return doc.getSize()<=Integer.parseInt(values[1]) && doc.name.contains(values[0]);}
            else if (operator[1].equals("<")) {return doc.getSize()<Integer.parseInt(values[1]) && doc.name.contains(values[0]);}
            else if(operator[1].equals("==")){return doc.getSize()==Integer.parseInt(values[1]) && doc.name.contains(values[0]);}
            else if (operator[1].equals("!=")){return doc.getSize()!=Integer.parseInt(values[1]) && doc.name.contains(values[0]);}
        }
        else if(cri.getAttrName().equals("type&&size")){
            String OperatorWithoutBrackets = cri.getOp().substring(1, cri.getOp().length() - 1);
            String[] operator= OperatorWithoutBrackets.split(", "); //type <
            String Values = cri.getVal().substring(1, cri.getVal().length() - 1);
            String[] values = Values.split(", ");// txt 100
            if(operator[1].equals(">")){return doc.getSize()>Integer.parseInt(values[1]) && doc.getType().equals(values[0]); }
            else if (operator[1].equals(">=")) {return doc.getSize()>=Integer.parseInt(values[1]) && doc.getType().equals(values[0]);}
            else if(operator[1].equals("<=")) {return doc.getSize()<=Integer.parseInt(values[1]) && doc.getType().equals(values[0]);}
            else if (operator[1].equals("<")) {return doc.getSize()<Integer.parseInt(values[1]) && doc.getType().equals(values[0]);}
            else if(operator[1].equals("==")){return doc.getSize()==Integer.parseInt(values[1]) && doc.getType().equals(values[0]);}
            else if (operator[1].equals("!=")){return doc.getSize()!=Integer.parseInt(values[1]) && doc.getType().equals(values[0]);}
        }
        else if(cri.getAttrName().equals("size&&type")){
            String OperatorWithoutBrackets = cri.getOp().substring(1, cri.getOp().length() - 1);
            String[] operator= OperatorWithoutBrackets.split(", "); //type <
            String Values = cri.getVal().substring(1, cri.getVal().length() - 1);
            String[] values = Values.split(", ");// txt 100
            if(operator[0].equals(">")){return doc.getSize()>Integer.parseInt(values[0]) && doc.getType().equals(values[1]); }
            else if (operator[0].equals(">=")) {return doc.getSize()>=Integer.parseInt(values[0]) && doc.getType().equals(values[1]);}
            else if(operator[0].equals("<=")) {return doc.getSize()<=Integer.parseInt(values[0]) && doc.getType().equals(values[1]);}
            else if (operator[0].equals("<")) {return doc.getSize()<Integer.parseInt(values[0]) && doc.getType().equals(values[1]);}
            else if(operator[0].equals("==")) {return doc.getSize()==Integer.parseInt(values[0]) && doc.getType().equals(values[1]);}
            else if (operator[0].equals("!=")) {return doc.getSize()!=Integer.parseInt(values[0]) && doc.getType().equals(values[1]);}
        }
        else if(cri.getAttrName().equals("type&&name")){
            String OperatorWithoutBrackets = cri.getOp().substring(1, cri.getOp().length() - 1);
            String[] operator= OperatorWithoutBrackets.split(", "); // > contains
            String Values = cri.getVal().substring(1, cri.getVal().length() - 1);
            String[] values = Values.split(", ");
            return (doc.getType().equals(values[0])&&doc.name.contains(values[1]));
        }
        else if (cri.getAttrName().equals("name&&type")){
            String OperatorWithoutBrackets = cri.getOp().substring(1, cri.getOp().length() - 1);
            String[] operator= OperatorWithoutBrackets.split(", "); // > contains
            String Values = cri.getVal().substring(1, cri.getVal().length() - 1);
            String[] values = Values.split(", ");
            return (doc.getType().equals(values[1])&&doc.name.contains(values[0]));
        }
        return true;
    }

}
