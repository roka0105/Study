package yeram_day1;
import java.util.Scanner;
/*string in ���� �޾ƿ� �����ʹ� ����Ʈ ��Ʈ�� �������̹Ƿ� scanner ����Ͽ� ��ȯ������Ѵ�.
 * ��ĳ�� �ȿ� ���۰� �ִµ� �� ���ۿ� �Է��� Ű�� ������ �� ���� �ϳ��ϳ� �������°� next()�żҵ�
 * enter �Է��ϱ� �������� �����Ͱ� ��� ���ۿ� ��.
 * ex) a sapce 10 space Hello enter �ϰ� next() ����� a, 10 , Hello �� ���ʷ� �����´�.
 * space�� ������ ���� ��ū�̶��ϴµ� next�� ��ū���� �����´�.
 * nextline()�� ���ۿ��� \n ���� ��°�� �о�´�.
 * close() scanner ������� Ű����� ���� ����. */

public class ScannerEx 
{

	public static void main(String[] args) 
	{
		// TODO Auto-generated method stub
		System.out.println("�̸�,����,����,ü��,���� ���θ� ��ĭ���� �и��Ͽ� �Է��Ͻÿ�.");
		Scanner scanner=new Scanner(System.in);
		
		String name= scanner.next();
		System.out.println("�̸�:"+name+"\n");
		
		String city= scanner.next();
		System.out.println("����:"+city+"\n");
		
		int age=scanner.nextInt();
		System.out.println("����:"+age+"\n");
		
		double weight =scanner.nextDouble();
		System.out.println("ü��:"+weight+"\n");
		
		boolean single= scanner.nextBoolean();
		System.out.println("����:"+single+"\n");
		
		scanner.close();
		
	}

}
