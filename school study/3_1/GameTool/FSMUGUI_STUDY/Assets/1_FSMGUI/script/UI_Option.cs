using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace FSMGUI_NUM1
{
    public class UI_Option : MonoBehaviour, IState
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