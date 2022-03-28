package yeram_day1;
import java.util.Scanner;
public class GradingSwitch 
{

	public static void main(String[] args) 
	{
		// TODO Auto-generated method stub
        Scanner scanner =new Scanner(System.in);
        char grade;
        System.out.print("점수를 입력하시오");
        int score=scanner.nextInt();
        switch(score/10)
        {
        case 10:
        case 9:
        	grade='A';
        	break;
        case 8:
        	grade='B';
        	break;
        case 7:
        	grade='C';
        	break;
        case 6:
        	grade='D';
        	break;
        default:
       		grade='F';
       		break;
        		
        }
        System.out.println("학점은"+grade);
        scanner.close();
	}

}
