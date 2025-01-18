public class Set <T extends Comparable<T>>{
    private T [] data;
    private int size;
    public Set(int capacity){
        data = (T[])new Comparable[capacity];
        size = 0;
    }
    public String toString(){
        StringBuilder res = new StringBuilder();
        for(T ele : data){
            res.append(ele);
        }
        return res.toString();
    }
    public boolean has(T element){
        for(T ele : data){
            if(ele.equals(element)) return true;
        }
        return false;
    }
    public void add(T element){
        if(size==data.length) return;
        if(has(element)) return;
        data[size] = element;
        size++;
    }

    public static void main(String[] args) {

    }
}
class A7 {
    public static class B7{
        private int l;
        public B7(int a){l = a;}
    }
    B7 h = new B7(2);



}