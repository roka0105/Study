using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class UIBattle : ElementBaseForm<E_UIState>
	{
        //public override E_UIState EnumType
        //{
        //    get
        //    {
        //        return E_UIState.battle;
        //    }
        //}
        public override E_UIState EnumType => E_UIState.battle;

        void Start()
		{
			
		}

	}
}