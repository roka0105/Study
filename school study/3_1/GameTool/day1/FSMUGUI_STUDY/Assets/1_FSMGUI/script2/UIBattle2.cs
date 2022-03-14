using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace yeram
{
    public class UIBattle2 : MonoBehaviour,IState
    {
        public void OnEnter(object data)
        {
            gameObject.SetActive(true);
        }

        public void OnExit()
        {
            gameObject.SetActive(false);
        }

        public void StateRender()
        {
           
        }

        public void StateUpdate()
        {
            
        }

        // Start is called before the first frame update
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }
    }
}

