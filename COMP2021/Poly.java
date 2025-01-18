import java.util.List;

class Animal{
    protected String name;
    public Animal(String name){
        this.name = name;
    }
    protected void greet(){
        System.out.println("Animal");
    }
}
class Dog extends Animal{
    public Dog(String name) {
        super(name);
    }

    @Override
    public void greet() {
        System.out.println("My name is "+ name +" I am a Dog");
    }
    public void play(){
        System.out.println("eat shit");
    }
}
class Cat extends Animal{

    public Cat(String name) {
        super(name);
    }
    @Override protected void greet(){
        System.out.println("I am a cat.");
    }
}

abstract class Abstract{
    public abstract void print(String s);

}

class SomeClass extends Abstract{
    @Override
    public void print(String s) {
        System.out.println("The content is: " + s);
    }
}

interface Behavior{
    public void play();
    public void Run();
}

class Student implements Behavior{
    @Override
    public void play() {
        System.out.println("Play..");
    }

    @Override
    public void Run() {
        System.out.println("Running..");
    }
}
public class Poly {
    public static void main(String[] args) {
        Cat c = new Cat("Cyx");
        Animal a = (Animal) c; // 从子类casting 父类，不报错
        Animal a2 = new Animal("CAOYIXIN");
       // Dog d = (Dog) a2; //runtime error,从父类不能casting子类！(括号里面不能是子类)
        System.out.println(a instanceof Cat); // true
        System.out.println(c instanceof Animal); // true

        Dog d = new Dog("TKT");
        System.out.println(d instanceof Dog);
        Dog d5 = new Dog("j");
        Animal h = d5;

    }
}