using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace yeram
{
    public class UIManager2 : MonoBehaviour
    {
        public E_State cur_state=E_State.world;
        StateMachine2 stateMachine=new StateMachine2();
        void Init()
        {

            UIWorld2 world = GameObject.FindObjectOfType<UIWorld2>();
            world.gameObject.SetActive(false);
            stateMachine.Add(E_State.world.ToString(), world);

            UIBattle2 battle = GameObject.FindObjectOfType<UIBattle2>();
            battle.gameObject.SetActive(false);
            stateMachine.Add(E_State.battle.ToString(), battle);

            ChangeState();
        }
        [ContextMenu("상태변환")]
        void Edit_Change()
        {
            ChangeState();
        }
        void ChangeState()
        {
            stateMachine.Change(cur_state.ToString(), null);
        }
        private void Awake()
        {
            Init();
        }
        // Start is called before the first frame update
        void Start()
        {
        }

        // Update is called once per frame
        void Update()
        {
            stateMachine.StateUpdate();
        }
    }
}

