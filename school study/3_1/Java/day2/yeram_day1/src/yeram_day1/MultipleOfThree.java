package yeram_day1;
import java.util.Scanner;
public class MultipleOfThree 
{

	public static void main(String[] args)
	{
     Scanner scanner=new Scanner(System.in);
     System.out.print("���� �Է��Ͻÿ�.");
     int number=scanner.nextInt();
     if(number%3==0)
    	 System.out.println("3�� ���");
     else System.out.println("3�� ����� �ƴ�.");
     scanner.close();
	}
}