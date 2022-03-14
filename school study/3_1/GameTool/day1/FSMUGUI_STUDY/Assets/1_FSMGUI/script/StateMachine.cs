using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace FSMGUI_NUM1
{
    public interface IState
    {
        void UpdateState(float time);
        void Render();
        void OnEnter(object data);
        void OnExit();
    }
    public class StateMachine : MonoBehaviour
    {
        Dictionary<string, IState> mStates = new Dictionary<string, IState>();
        IState CurrentState = null;
        public bool StateNullCheck
        {
           get { return CurrentState != null; }
        }
        public void UpdateState(float time)
        {
            CurrentState.UpdateState(time);
        }
        public void Render()
        {
            CurrentState.Render();
        }
        public void Change(string name,object data)
        {
            if(StateNullCheck)
            CurrentState.OnExit();
            CurrentState = mStates[name];
            CurrentState.OnEnter(data);
        }
        public void Add(string name,IState obj)
        {
            mStates[name] = obj;
        }
    }

}
