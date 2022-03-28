package yeram_day1;

public class CircleArea 
{
	public void sum(int n,int m)
	{
		System.out.println(n+m);
	}
    
	public static void main(String[] args) 
	{
		/* 매소드를 static 없이 생성할시 class 객체를 생성한뒤에 접근하도록 해야한다.
		 * cpp처럼 class와 main은 떨어져있다고 생각하면 되지만 완전한 객체지향을 위해 class 안에 넣어두기만 한 것임.
		 * main의 실행명령을 수행하기 위해 class 에있는 매소드 접근시 객체 생성.*/
		CircleArea m=new CircleArea();
		m.sum(1,2);
		final double PI=3.14;
		double radius =10.0;
		double circleArea=radius*radius*PI;
		System.out.println("원의 면적 ="+circleArea);

	}

}
