using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System;
using UnityEngine;

/*dontdestroy 안한 이유는 게임이 끝나면 씬이 변하고 새 게임을 선택하게 될텐데 일일히 데이터를 초기화 하지 않기 위해서.
  어차피 게임 진행은 한 씬에서 이루어 질 테니까 해당 게임이 끝나면 매니저들에 담긴 데이터 정보들이 자동 초기화 되도록.
 */
namespace Singleton_Ver1
{
	public class Singleton<T> : MonoBehaviour where T : MonoBehaviour
	{
		[SerializeField]
		protected bool flag;
		private static T instance;
		public static T Instance
		{
			get
			{
				Singleton_Ver1.Singleton<T>[] objs = FindObjectsOfType<Singleton_Ver1.Singleton<T>>(true);
				GameObject obj = objs.Where(item => item.flag == true).FirstOrDefault().gameObject;
				if(obj == null)
				{
					if(objs.Length>0)
					{
						return objs[0].GetComponent<T>();
					}
					obj = new GameObject(typeof(T).Name);
					obj.AddComponent<T>();
				}
				else
				{
					instance = obj.GetComponent<T>();
				}
				return instance;
			}
		}
	}

}

//Lazy singleton 멀티 스레드 환경에서 lock대신 사용하는데 선언 즉시 생성이 아니라 호출 시점에 생성,생성 시점을 조절 할 수 있음.
/*1.생성이 오래걸리는 큰 오브젝트 필요할때만 생성하고 싶을때
  2.리소스를 많이 사용하는 실행을 필요할때만 할때
  3.자원 생성을 멀티 스레드 환경에서 안전하게 해야 할 때
  이해가 될락말락 잘 모르겠다. 여쭤보기
  만약에 서버를 넣게 되면 클라에도 프로그램 스레드,샌드 스레드,리시브 스레드 세가지로 나뉘게 되면
  예를 들어 내가 보스 타입이 3가지가 있는데 1가지가 랜덤으로 걸리는 시스템이라면 클라에서 보스 타입을 결정해서
  서버에 요청하고 게임 들어가기 전에 로딩 시간동안 해당 보스에 대한 해당 데이터를 로드받아서 게임정보를 저장하고 있을 매니저에 넣을때
  사용하는거 아닌가? 이처럼 스테이지별로 게임을 구현한다면 해당 스테이지에서 필요한 
  받아야할 정보만 클라에서 서버로 요청하고 서버에서 클라로 데이터를 받을때 해당 데이터 매니저를 생성하는 것 아닐까..
  해당 스테이지에 필요 없는 데이터까지 한꺼번에 로드하면 안되니까.
  */
namespace Singleton_Ver2
{
	public class Singleton<T> : MonoBehaviour where T : MonoBehaviour
	{
		[SerializeField]
		protected bool flag;
		private static readonly Lazy<T> _instance = 
		new Lazy<T>(() =>
		{
			Singleton_Ver2.Singleton<T>[] objs = FindObjectsOfType<Singleton_Ver2.Singleton<T>>(true);
			GameObject obj = objs.Where(item => item.flag == true).FirstOrDefault().gameObject;
			if(obj==null)
			{
				if(objs.Length>0)
				{
					return objs[0].GetComponent<T>();
				}
				obj = new GameObject(typeof(T).Name);
				obj.AddComponent<T>();
			}	
			return obj.GetComponent<T>();
		});
		public static T Instance
		{
			get
			{
				return _instance.Value;
			}
		}
	}
}