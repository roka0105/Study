using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class UI_Battle : MonoBehaviour, IState
	{
        public void OnEnter(object p_data)
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

        public void UpdateState(float elapsedTime)
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