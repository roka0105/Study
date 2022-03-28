package yeram_day1;

public class Hello 
{
    /*java 객체지향 프로그래밍 
     * 객체-> class=설계도 instance=실물
     * 컴파일시 hello.class를 하드디스크에 저장.
     * 컴파일시 명령과 명령실행에 필요한 데이터를 분리해서 하드에 저장.(소스코드(=설계도)가 저장됨.)
     * 실행시 하드디스크에 있는 클래스(설계도)가 main memory에 올라옴.
     * 메인 메모리에 올라갈때 명령이 올라가는 영역은 code 영역, 클래스가 올라가는 영역은 heap
     * 스택영역에 메인매소드가 사용할 방을 만들어둔다.
     * 이때 메인의 지역변수는 그 방에 위치한다.
     * (메인메모리에 올라갔다는건 주소 할당받았다는것)
     * 클래스. 으로 접근하는건 클래스가 위치한 주소에 접근하는것이다. 클래스 이름= 주소 =레퍼런스.
     * cpu가 code 영역에서 명령을 뽑아와서 실행하고 필요한 데이터는 stack에서 뽑아온다.
     * sum도 메소드이기 때문에 stack에 sum 방이 생긴다. 이곳에 필요한 매개변수,지역변수를 위치시킨다.
     * return 된 값은 main 메소드의 변수에 들어가고 실행이 끝나면 sum은 stack프레임에서 사라진다.
     * println도 매소드이기때문에 stack에 방이 생기고 우리가 사용할 a값이 매개변수로 copy되어서 전달되고,
     * 실행이 끝나면 사라진다. main도 모든 수행이 끝나면 stack에서 사라진다. 
     * 프로그램이 종료되며 code/heap/stack 영역에 있는 모든 정보가 사라진다.
     * 하드에 저장된 소스코드의 class 는 설계도이고 main memory heap에 저장된 class 는 틀이다.
     * 
     * 객체 지향의 대원칙
     * 1.instance를 생성한 후에 필드나 매소드를 사용(참조).
     * 2.이를 지키지 않는 경우는 표시해야한다->static
     *   instance를 만들지 않고 class 안에 있는 것을 쓸수 있게 함.
     *   
     * c#이나 java는 main도 클래스 안에 존재해야함
     * 
     * 상수 값을 만들땐 final 사용한다. c++에서는 const 사용.
     * 상수 선언 
     * final class 는 상속 불가 final 매소드는 오버라이딩 불가
     * 
     * 정수 x 배열은 힙영역에 잡힘. main에 선언됐다면 stack의 선언된 x는 힙에 잡힌 x배열의 값의 주소를 참조해서 받아옴.
     * ex)main -> int x=new int[4]; 
     * 
     * unsigned는 java에 없다.(암호화할때 양수만 쓰기때문에 짤때 좀 불편해질 수 있음)
     * 
     * 정수 리터럴
     * 10진수 15
     * 8진수 015 ->십진수로 13
     * 16진수 0x15->십진수로 21
     * 2진수 0b0101->십진수로 5
     * 
     * 실수 리터럴
     * float -> 0.1f(4byte)
     * double -> 0.1D 또는 특별한 말 없으면 걍 double로 처리함 (8byte)
     * 
     * 코드값하고 인코딩 결과값은 다르다.
     * 'A'는 16진수로 41(코드값)을 인코딩해서 2진수로 변환하는데 인코딩 방식에 따라 바뀌는게 다르다.
     * utf8 인코딩 =>'A'->01000001-> 01000001 8bit
     * utf16 인코딩 =>'A'->00000000 01000001 16bit로 만듦
     * utf32 인코딩 =>
     * 인코딩 방식이 달라지면 다른 2진수가 나온다.
     * 
     * 숫자 중간에 _ 넣을수 있다 컴파일시 _를 제거해줌 하지만 소수점 앞이나 숫자 끝에 사용하는 등은 불가능.
     * 
     * var x= 20; var는 컴파일러가 값을 보고 타입을 정하도록 한다.
     * var ob=new abc() var=>abc* 
     */
	public static int sum(int n,int m)
	{
		return n+m;
	}
	public static void main(String[] args)
	{
		// TODO Auto-generated method stub
		int i=20;
		int s;
		char a;
		s= Hello.sum(i,10);
		a='?';
		System.out.println(a);
		System.out.println("Hello");
		System.out.println(s);
		System.out.printf("abc %d", 123);
		
	}

}
