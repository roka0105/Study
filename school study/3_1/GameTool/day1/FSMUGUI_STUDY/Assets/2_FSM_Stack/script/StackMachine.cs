using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace FSMGUI_NUM1
{
    public class StackMachine
    {
        Dictionary<string, IState> dic_state = new Dictionary<string, IState>();
        Stack<IState> State_Stack = new Stack<IState>();

        public void StateUpdate()
        {
            IState top = State_Stack.Peek();
            top.UpdateState(0);
        }
        public void StateRender()
        {
            IState top = State_Stack.Peek();
            top.Render();
        }
        public void Add(string name, IState state)
        {
            if (dic_state.ContainsKey(name))
            {
                dic_state[name] = state;
            }
            else dic_state.Add(name, state);
        }
        public IState Push(string name)
        {
            if (State_Stack.Count > 0)
            {
                IState top = State_Stack.Peek();
                top.OnExit();
            }
            IState state = dic_state[name];
            State_Stack.Push(state);
            state.OnEnter(null);
            return state;
        }
        public IState Pop()
        {
            if (State_Stack.Count <= 1)
            {
                return null;
            }
            IState pop = State_Stack.Pop();
            pop.OnExit();
            IState top = State_Stack.Peek();
            top.OnEnter(null);

            return pop;
        }

    }
}

