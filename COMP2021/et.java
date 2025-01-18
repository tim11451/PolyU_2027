public class et {
    private static void statement(int number) throws ET1, ET2 {
        switch(number){
            case 1:
                throw new ET1();
            case 2:
                throw new ET2();


        }
    }
    public static void f(){
        try{statement(1); }
        catch (ET1 | ET2 e) {
            throw new RuntimeException(e);
        }
    }
    public static void main(String[] args) {
        f();
    }
}
class ET1 extends Exception{
    public ET1(){
        super("1");
    }
}
class ET2 extends Exception{
    public ET2(){
        super("2");
    }
}

class D implements Comparable<D>{
    private int age;

    @Override
    public int compareTo(D o) {
        return Integer.compare(this.age,o.age);
    }

}
class A {
    public void play(int i, int m){}
}
class B extends A {
    @Override public void play(int i, int m){

    }
}