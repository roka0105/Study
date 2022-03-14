using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class Code_StateStackMachine
	{
        Dictionary<string, IState> mStates = new Dictionary<string, IState>();
        Stack<IState> m_Stack = new Stack<IState>();

        public void Update(float elapsedTime)
        {
            IState top = m_Stack.Peek();
            top.UpdateState(elapsedTime);
        }

        public void Render()
        {
            IState top = m_Stack.Peek();
            top.Render();
        }

        public IState Push(string name)
        {
            if(m_Stack.Count > 0)
            {
                IState top = m_Stack.Peek();
                top.OnExit();
            }
            

            IState state = mStates[name];
            m_Stack.Push(state);
            state.OnEnter(null);

            return state;
        }

        public IState Pop()
        {
            if (m_Stack.Count <= 1)
            {
                return null;
            }

            IState pop = m_Stack.Pop();
            pop.OnExit();

            IState top = m_Stack.Peek();
            top.OnEnter(null);

            return pop;
        }

        public void Add(string p_name, IState p_state)
        {
            if( mStates.ContainsKey(p_name) )
            {
                mStates[p_name] = p_state;
            }
            else
            {
                mStates.Add(p_name, p_state);
            }
            
        }

	}
}