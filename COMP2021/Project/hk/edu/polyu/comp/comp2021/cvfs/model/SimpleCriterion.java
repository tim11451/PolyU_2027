package hk.edu.polyu.comp.comp2021.cvfs.model;

//simplecriterion类
public class SimpleCriterion {
    private  String criName;
    private String attrName;
    private String op;
    private String val;

    public SimpleCriterion(String criName,String attrName, String op, String val) {
        this.criName = criName;
        this.attrName = attrName;
        this.op = op;
        this.val = val;
    }

    public String getAttrName() {
        return attrName;
    }

    public void setAttrName(String attrName) {
        this.attrName = attrName;
    }

    public String getOp() {
        return op;
    }

    public void setOp(String op) {
        this.op = op;
    }

    public String getVal() {
        return val;
    }

    public void setVal(String val) {
        this.val = val;
    }

    public String getCriName(){return criName;}

    public void setCriName(String name){this.criName = name;}
}
