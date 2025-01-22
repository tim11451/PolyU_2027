package hk.edu.polyu.comp.comp2021.cvfs.model;

public class Document extends File {
    private String type;
    private String content;

    public Document(String name,String type,String content) {
        super(name);
        if(Valid.isValidType(type)){
            setType(type);
        }
        setContent(content);
    }

    public String getType() {
        return type;
    }
    public String getContent() {
        return content;
    }

    public void setType(String type) {
        this.type = type;
    }

    public void setContent(String content) {
        this.content = content;
    }

    @Override
    public int getSize(){
        return 40 + content.length() * 2;
    }
}
