using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace FSMGUI_NUM1
{
    public class UI_Battle : MonoBehaviour, IState
    {
        public void OnEnter(object data)
        {
            gameObject.SetActive(true);
        }

        public void OnExit()
        {
            gameObject.SetActive(false);
        }

        public void Render()
        {
           
        }

        public void UpdateState(float time)
        {
          
        }
        void Awake()
        {

        }
        void Start()
        {

        }
    }
}

