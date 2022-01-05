using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Office.Interop.Excel;
using Excel = Microsoft.Office.Interop.Excel;

namespace make_excel
{
	class Program
	{
		static void Main(string[] arg)
		{
			string filepath =
				@"C:\Users\user\OneDrive\바탕 화면\Table_List";

			string filename = "01_Devil_Table.xlsx";
			string sheetname ="Devil_Table";
			if(!filepath.EndsWith("\\"))
				filepath+="\\";
			ExcelReader excel = new ExcelReader();
			excel._Initialize(filepath + filename);
		}
	}
	class ExcelReader
	{
		public struct Pos
		{
			public int row;
			public int col;
			public Pos(int r,int c)
			{
				row = r; 
				col = c;
			}
		}
		Application m_app = null;
		Workbook m_workbook = null;
		string filepath;
		bool IsFinalized = false;
		public ExcelReader()
		{ 
		}
		~ExcelReader()
		{
			if (!IsFinalized)
				_Finalize();
		}
		public bool _Initialize(string path)
		{
			m_app = new Application();
			m_workbook = m_app.Workbooks.Open(path);

			filepath = path;
			return true;
		}
		public void _Finalize()
		{
			IsFinalized = true;
			//Type.Missing =기본으로 설정된 인자값 사용됨.
			m_workbook.Close(false, Type.Missing, Type.Missing);
			m_app.Quit();

			DeleteObj(m_workbook);
			DeleteObj(m_app);
			
			GC.Collect();
		}
		public void DeleteObj(Object obj)
		{
			try
			{
				#region System.Runtime.InteropServices.Marshal.ReleaseComObject() 설명
				/*지정된 com 개체와 연결된 지정된 RCW(런타임 호출 가능 래퍼)의 참조 횟수를 감소 시킨다. 
				  보통 1개씩 연결 유지되므로 반환값은 감소된 0
				  파라미터는 해제할 COM 개체이다.
				  엑셀 COM 객체들을 해제하지 않으면 exe파일이 남아서 실행중이라던가 그렇다.
				  .NET 에서 엑셀 객체들과 같은 COM 객체를 액세스 할 때는 항상 RCW(Runtime Callable Wrapper)를 통해 엑세스 된다.
				  RCW에서는 finalizer를 정의하고 있고 이 Finalizer가 COM 객체를 해제하도록 구성되어 있다.
			      따라서 엑셀 프로세스가 종료되는 시점은 다음 가비지 컬렉션이 수행된 후가 될 것이다.
				  하지만 가비지 수행 후 finalizer가 곧바로 수행된다는 보장이 없고 finalizer 메소드는 별도의 finalizer만을 위한 스레드가
				  호출 되도록 되어있어 언제 호출될지 알 수 없다.
				  따라서 명시적으로 해제를 해야한다. 
				  System.Runtime.InteropServices.Marshal.ReleaseComObject()  사용.
				  강제로 가비지 수행후 RCW Finalizer 호출되도록 하는 방법이 있지만 이는 좋지 않다.
				  GC.Collect() 같은것으로 Excel.exe 프로세스를 종료하는 방법이 있지만 권장 X.
				+ 반복문 안에서 COM 객체를 참조하는 경우 해제 역시 반복문 내에서 이루어져야한다.
				https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=runtostar&logNo=80211201811
				 */
				#endregion
				System.Runtime.InteropServices.Marshal.ReleaseComObject(obj);
				obj = null;
			}
			catch
			{
				obj= null;
				Console.WriteLine("Delete Obj Error");
			}
			finally
			{
				#region GC 간단 설명
				/*가비지 컬렉터 호출하여 모든 데이터에 대한 처리 수행.
				세대값에 따라 정리한다. 0순위 데이터가 주로 많이 정리됨.
				0세대=최근에 생긴 데이터,1순위로 정리될 가능성이 큰 데이터
				0세대가 일정 비율보다 적어지면 1세대 데이터가 정리된다.
				0세대가 여러번 반복하면 1세대가 한번 수행
				1세대가 여러번 반복하면 2세대가 한번 수행되는 식이다.
				가비지 컬렉터에 안걸러지면 세대가 1씩 증가 최대 세대는 2이다.*/
				#endregion
				GC.Collect();
			}
		}
		public void ReadSheet(string sheetName)
		{

		}
		void WriteFile(Excel.Worksheet sheet,int left,int right,int top,int bottom)
		{

		}
		void CreateFile(string path)
		{

		}
		void SaveFile(string path,string text)
		{

		}
	}
}