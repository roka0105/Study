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
				@"C:\Users\user\Desktop\Table_List";

			string filename = "01_Devil_Table.xlsx";
			string sheetname ="Devil_Table";
			if(!filepath.EndsWith("\\"))
				filepath+="\\";
			ExcelReader excel = new ExcelReader();
			excel._Initialize(filepath + filename);
			excel.ReadSheet(sheetname);
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
		{   //원하는 시트 정보 불러오기
			Excel.Worksheet sheet = m_workbook.Worksheets.get_Item(sheetName);
			#region 읽어들일 정보의 구역 정보 가져오기
			Excel.HPageBreak hPagebreak_start = sheet.HPageBreaks[1];
			Excel.HPageBreak hPagebreak_end = sheet.HPageBreaks[2];
			Excel.VPageBreak vPagebreak_start = sheet.VPageBreaks[1];
			Excel.VPageBreak vPagebreak_end=sheet.VPageBreaks[2];
			#endregion
			#region 구역정보를 통해 읽어들일 셀의 범위 정보 저장.
			Excel.Range hbreak_start_range = hPagebreak_start.Location;
			Excel.Range hbreak_end_range = hPagebreak_end.Location;
			Excel.Range vbreak_start_range = vPagebreak_start.Location;
			Excel.Range vbreak_end_range=vPagebreak_end.Location;
			#endregion

			WriteFile(sheet,vbreak_start_range.Column, vbreak_end_range.Column, hbreak_start_range.Row, hbreak_end_range.Row);

			DeleteObj(hbreak_start_range);
			DeleteObj(hbreak_end_range);
			DeleteObj(vbreak_start_range);
			DeleteObj(vbreak_end_range);
			DeleteObj(hPagebreak_start);
			DeleteObj(hPagebreak_end);
			DeleteObj(vPagebreak_start);
			DeleteObj(vPagebreak_end);
			DeleteObj(sheet);
		}
		void WriteFile(Excel.Worksheet sheet,int left,int right,int top,int bottom)
		{
			int idx = filepath.LastIndexOf("\\");
			string sheetName = sheet.Name;
			string structName = sheetName + "Excel";
			string savepath_cs = System.IO.Path.Combine(filepath.Substring(0, idx), structName + "Loader" + ".cs");
			string savepath_txt = System.IO.Path.Combine(filepath.Substring(0, idx), sheetName + ".txt");

			StringBuilder sb = new StringBuilder();

			List<string>types=new List<string>();
			List<string> names = new List<string>();
			#region unity cs 파일 생성
			sb.Length = 0;
			#region include
			sb.Append("using System.Collections.Generic;\n");
			sb.Append("using UnityEngine;\n\n");
			#endregion
			#region struct 생성
			sb.Append("[Sysyem.Serializable]\n");
			sb.Append("public struct ");
			sb.Append(structName);
			sb.Append("\n{\n");
			
		    for(int i=left;i<right;i++)
			{   
				//뽑아올 데이터의 형 (int string float) 문자열을 뽑아온다.
				Excel.Range typename_range = sheet.Cells[top + 1, i];
				Excel.Range dataname_range = sheet.Cells[top, i];

				string typename=typename_range.Value2.ToString();
				string dataname=dataname_range.Value2.ToString();
				types.Add(typename);
				names.Add(dataname);

				sb.Append("\tpublic ");
				sb.Append(typename);
				sb.Append(' ');
				sb.Append(dataname);
				sb.Append(";\n");

				//*사용한 excel객체는 그때그때 반환해줘야 한다.
				DeleteObj(typename_range);
				DeleteObj(dataname_range);
			}
			sb.Append("}\n\n");
			#endregion
			sb.Append("\n\n/*====================================*/\n\n");
			#region scriptable object && [CreateAssetMenu]
			sb.Append("[CreateAssetMenu(fileName=\"" + sheetName + "Loader" + "\", menuName= " + "\"Scriptable Object/" + sheetName + "Loader" + "\")]\n");
			sb.Append("public class " + structName + "Loader " + ":ScriptableObject\n");
			sb.Append("{\n");

			string tab = "";
			tab += '\t';
			//member
			sb.Append(tab + "[SerializeField] string filepath;\n");
			sb.Append(tab + "public List<" + structName + "> DataList;\n\n");
			//read func
			sb.Append(tab + "private " + structName + " Read(string line)\n");
			sb.Append(tab + "{\n");
			
			tab += '\t';
			sb.Append(tab + "line = line.TrimStart('\n');\n\n");

			sb.Append(tab + structName + " data = new "+structName+"();\n");
			sb.Append(tab + "int idx =0;\n");
			sb.Append(tab + "string[] strs= line.Split('`');\n\n");

			for(int i=0;i<names.Count;++i)
			{
				//member data set
				switch(types[i])
				{
					case "string":
						sb.Append(tab + "data." + names[i] + " = strs[idx++];");
						break;
					default:
						sb.Append(tab + "data." + names[i] + " = " + types[i] + ".Parse(strs[idx++]);");
						break;
				}
				sb.Append("\n");
			}
			sb.Append("\n");
			sb.Append(tab + "return data;\n");
			tab = tab.Remove(tab.Length - 1);
			sb.Append(tab + "}\n");
			#endregion
			#region file read all (파일의 모든 정보 읽어오기)
			sb.Append(tab + "[ContextMenu(\"파일 읽기\")]\n");
			sb.Append(tab + "public void ReadAllFile()\n");
			sb.Append(tab + "{\n");

			tab += '\t';
			sb.Append(tab + "DataList=new List<" + structName + ">();\n\n");

			sb.Append(tab + "string currentpath = System.IO.Directory.GetCurrentDirectory();\n");
			sb.Append(tab + "string allText = System.IO.File.ReadAllText(System.IO.Path.Combine(currentpath,filepath));\n");
			sb.Append(tab + "string[] strs = allText.Split(';');\n\n)");

			sb.Append(tab + "foreach (var item in strs)\n");
			sb.Append(tab + "{\n");
			tab += '\t';
			sb.Append(tab + "if(item.Length<2)\n");
			sb.Append(tab + '\t' + "continue;\n");
			sb.Append(tab + structName + " data = Read(item);\n");
			sb.Append(tab + "DataList.Add(data);\n");
			tab=tab.Remove(tab.Length - 1);
			sb.Append(tab + "}\n");
			tab = tab.Remove(tab.Length - 1);
			sb.Append(tab + "}\n");
			tab = tab.Remove(tab.Length - 1);
			sb.Append(tab + "}\n");
			//해당 문자열을 담은 .cs 파일을 만들어서 저장.
			CreateFile(savepath_cs);
			SaveFile(savepath_cs, sb.ToString());
			#endregion

			#region file info -> txt // 파일 정보를 cvs 파일로 만들어 저장.
			sb.Length = 0;
			for(int i=top+2;i<bottom;++i)
			{
				for(int j=left;j<right;++j)
				{
					try
					{
						Excel.Range r = sheet.Cells[i, j];
						sb.Append(r.Value2.ToString());
						sb.Append('`');
						DeleteObj(r);
					}
					catch(Exception ex)//Exception 어플리케이션 실행도중 발생하는 오류목록.
					{
						Console.WriteLine(i + "번 줄, " + j + "번 칸");
						throw;
					}
				}
				//마지막 `를 지워줌
				--sb.Length;
				sb.Append(';');
				sb.Append("\n");
			}
			//마지막 줄의 ; 를 지워줌
			--sb.Length;
			//txt 파일로 저장 = excel정보를 csv 파일로 변환하여 저장.
			CreateFile(savepath_txt);
			SaveFile(savepath_txt, sb.ToString());
			#endregion
			#endregion

		}
		void CreateFile(string path)
		{
			if(!System.IO.File.Exists(path))
			{
				FileStream stream=System.IO.File.Create(path);
				stream.Close();
			}

		}
		void SaveFile(string path,string text)
		{
			System.IO.File.WriteAllText(path, text);
		}
	}
}