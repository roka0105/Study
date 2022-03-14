using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class Test_UISceneData : MonoBehaviour
	{
        public E_UIState UIStateType = E_UIState.battle;

        void Start()
		{
            UI_InGameManager.GetI.SetChangeScene(UIStateType);


        }


        [ContextMenu("[씬수정하기]")]
        void Editor_ChangeScene()
        {
            UI_InGameManager.GetI.SetChangeScene(UIStateType);

        }

		void Update()
		{
			
		}
	}
}