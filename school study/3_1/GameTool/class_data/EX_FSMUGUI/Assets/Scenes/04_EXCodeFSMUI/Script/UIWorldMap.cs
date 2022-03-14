using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class UIWorldMap : ElementBaseForm<E_UIState>
    {
        public override E_UIState EnumType => E_UIState.worldmap;

        void Start()
		{
			
		}

        public override void EnterForm()
        {
            // 소스로 작업 해두면 됨
            base.EnterForm();
            
        }

        public override void LeaveForm()
        {
            // 소스로 작업 해두면 됨
            base.LeaveForm();
            
        }

        //void Update()
        //{

        //}
    }
}