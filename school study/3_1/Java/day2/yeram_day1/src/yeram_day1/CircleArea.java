package yeram_day1;

public class CircleArea 
{
	public void sum(int n,int m)
	{
		System.out.println(n+m);
	}
    
	public static void main(String[] args) 
	{
		/* �żҵ带 static ���� �����ҽ� class ��ü�� �����ѵڿ� �����ϵ��� �ؾ��Ѵ�.
		 * cppó�� class�� main�� �������ִٰ� �����ϸ� ������ ������ ��ü������ ���� class �ȿ� �־�α⸸ �� ����.
		 * main�� �������� �����ϱ� ���� class ���ִ� �żҵ� ���ٽ� ��ü ����.*/
		CircleArea m=new CircleArea();
		m.sum(1,2);
		final double PI=3.14;
		double radius =10.0;
		double circleArea=radius*radius*PI;
		System.out.println("���� ���� ="+circleArea);

	}

}
