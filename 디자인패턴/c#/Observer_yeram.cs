using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//observer pattern 공부를 위해 짜본것 push하지말고 따로 빼두기

namespace MyEvent
{
	public abstract class Event
	{
		public abstract void _Update();
	}
}

public interface IEventFuc
{
	void AddEvent(MyEvent.Event e);
	void RemoveEvent(MyEvent.Event e);
	void Trigger();
}
public class Obj1 : MyEvent.Event
{
	public override void _Update()
	{
		Debug.Log("Hello");
	}
}
public class Obj2 : MyEvent.Event
{
	public override void _Update()
	{
		Debug.Log("Hi");
	}
}
public class Obj3 : MyEvent.Event
{
	public override void _Update()
	{
		Debug.Log("Bye");
	}
}
public class Observer_yeram : MonoBehaviour, IEventFuc
{
	List<MyEvent.Event> events = new List<MyEvent.Event>();

	public void AddEvent(MyEvent.Event e)
	{
		events.Add(e);
	}
	public void RemoveEvent(MyEvent.Event e)
	{
		events.Remove(e);
	}
	public void Trigger()
	{
		foreach (var e in events)
		{
			e._Update();
		}
	}
	void Start()
	{
		Obj1 obj1 = new Obj1();
		Obj2 obj2 = new Obj2();
		AddEvent(obj1);
		AddEvent(obj2);
		Trigger();
		RemoveEvent(obj2);
		Trigger();
	}


	void Update()
	{

	}

}
