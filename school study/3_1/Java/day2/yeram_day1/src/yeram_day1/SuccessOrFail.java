package yeram_day1;
import java.util.Scanner;
public class SuccessOrFail 
{

	public static void main(String[] args) 
	{
		// TODO Auto-generated method stub
        Scanner scanner =new Scanner(System.in);
        
        System.out.print("������ �Է��Ͻÿ�:");
        int score=scanner.nextInt();
        if(score>=80)
        	System.out.println("�����մϴ�! �հ��Դϴ�.\n");
        else System.out.println("���հ��Դϴ�.\n");
        scanner.close();
        
	}

}
