using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class UI_BattleForm : ElementBaseForm<E_UIState>
    {
        public override E_UIState EnumType{ get { return E_UIState.battle; } }

        public override void EnterForm()
        {
            base.EnterForm();
            gameObject.SetActive(true);
        }

        public override void LeaveForm()
        {
            gameObject.SetActive(false);
            base.LeaveForm();
        }

        void Start()
		{
			
		}

    }
}