using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class TestConsoleCls : MonoBehaviour
	{
        public int m_Test = 10;

        private int m_Test2 = 10;
        public int Test2
        {
            get { return m_Test2; }
            set { m_Test2 = value; }
        }

        public void ObjectSetActive(bool p_isflag)
        {
            gameObject.SetActive(p_isflag);
        }

		void Start()
		{
			
		}

		void Update()
		{
			
		}
	}
}