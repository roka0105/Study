package yeram_day1;
import java.util.Scanner;
/*string in 으로 받아온 데이터는 바이트 스트림 데이터이므로 scanner 사용하여 변환해줘야한다.
 * 스캐너 안에 버퍼가 있는데 이 버퍼에 입력한 키가 들어오고 이 값을 하나하나 가져오는게 next()매소드
 * enter 입력하기 전까지의 데이터가 모두 버퍼에 들어감.
 * ex) a sapce 10 space Hello enter 하고 next() 실행시 a, 10 , Hello 를 차례로 가져온다.
 * space로 구별된 것을 토큰이라하는데 next는 토큰별로 가져온다.
 * nextline()은 버퍼에서 \n 까지 통째로 읽어온다.
 * close() scanner 사용종료 키보드와 연결 끊음. */

public class ScannerEx 
{

	public static void main(String[] args) 
	{
		// TODO Auto-generated method stub
		System.out.println("이름,도시,나이,체중,독신 여부를 빈칸으로 분리하여 입력하시오.");
		Scanner scanner=new Scanner(System.in);
		
		String name= scanner.next();
		System.out.println("이름:"+name+"\n");
		
		String city= scanner.next();
		System.out.println("도시:"+city+"\n");
		
		int age=scanner.nextInt();
		System.out.println("나이:"+age+"\n");
		
		double weight =scanner.nextDouble();
		System.out.println("체중:"+weight+"\n");
		
		boolean single= scanner.nextBoolean();
		System.out.println("독신:"+single+"\n");
		
		scanner.close();
		
	}

}
