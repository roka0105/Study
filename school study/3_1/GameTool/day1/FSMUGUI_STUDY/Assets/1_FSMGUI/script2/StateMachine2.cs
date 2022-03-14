using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace yeram
{
    public interface IState
    {
        void StateUpdate();
        void StateRender();
        void OnEnter(object data);
        void OnExit();
    }
    [System.Serializable]
    class StateMachine2
    {
        IState CurrentState = null;
        Dictionary<string, IState> dic_state = new Dictionary<string, IState>();
        public void StateUpdate()
        {
            if (CurrentState != null)
                CurrentState.StateUpdate();
        }
        public void StateRender()
        {
            CurrentState.StateRender();
        }
        public void Add(string name, IState state)
        {
            dic_state[name] = state;
        }
        public void Change(string name, object obj)
        {
            if (CurrentState != null)
            {
                CurrentState.OnExit();
            }
            CurrentState = dic_state[name];
            CurrentState.OnEnter(obj);

        }
    }

}
