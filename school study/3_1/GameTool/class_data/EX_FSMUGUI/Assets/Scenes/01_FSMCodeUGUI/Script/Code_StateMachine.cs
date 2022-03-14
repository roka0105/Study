using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Du3Project
{
    public interface IState
    {
        void UpdateState(float elapsedTime);
        void Render();
        void OnEnter(object p_data);
        void OnExit();
    }

    [System.Serializable]
    public class EmptyState : MonoBehaviour, IState
    {
        public virtual void UpdateState(float elapsedTime)
        {
            // Nothing to update in the empty state.
        }

        public virtual void Render()
        {
            // Nothing to render in the empty state
        }

        public virtual void OnEnter(object p_data)
        {
            // No action to take when the state is entered
        }

        public virtual void OnExit()
        {
            // No action to take when the state is exited
        }
    }

    [System.Serializable]
    class StateMachine
    {
        Dictionary<string, IState> mStates = new Dictionary<string, IState>();
        IState mCurrentState = new EmptyState();

        public void UpdateState(float elapsedTime)
        {
            mCurrentState.UpdateState(elapsedTime);
        }

        public void Render()
        {
            mCurrentState.Render();
        }

        public void Change(string stateName, object p_data)
        {
            mCurrentState.OnExit();
            mCurrentState = mStates[stateName];
            mCurrentState.OnEnter(p_data);
        }

        public void Add(string name, IState state)
        {
            mStates[name] = state;
        }
    }


}