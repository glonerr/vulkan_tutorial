class People {
    People(){
        System.out.println("call people constructor");
    }

    protected void show() {
        System.out.println(a);
    }

    int a = 10;
}

class Student extends People {
    public class Box{
        public Box(){
            System.out.println("call student inner class box constructor");
        }
        int b = 20;
        public void show(){
            System.out.println(Student.this.a);
        }
    }

    Student(int a) {
        System.out.println("call student constructor");
    }

    public void show() {
        // System.out.println(this.b);
        super.show();
    }

    int a = 100;
}

class MyClass {
    public static void main(String[] args) {
        System.out.println("Hello World!"); // Display the string.
        People p = new Student(1);
        ((People) p).show();
    }
}