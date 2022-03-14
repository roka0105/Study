using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace FSMGUI_NUM1
{
    public class UIStackManager : MonoBehaviour
    {
        StackMachine Stack_machine = new StackMachine();
        public UI_Battle battle = null;
        public UI_Field field = null;
        public UI_Option option = null;
        public UI_World world = null;
        public E_UIState cur_state = E_UIState.world;
        void Init()
        {
            Stack_machine.Add(E_UIState.battle.ToString(), battle);
            Stack_machine.Add(E_UIState.field.ToString(), field);
            Stack_machine.Add(E_UIState.option.ToString(), option);
            Stack_machine.Add(E_UIState.world.ToString(), world);
            battle.gameObject.SetActive(false);
            field.gameObject.SetActive(false);
            option.gameObject.SetActive(false);
            world.gameObject.SetActive(false);
        }
        void ChangeState(E_UIState state)
        {
            Stack_machine.Push(state.ToString());
        }
        // Start is called before the first frame update
        void Start()
        {
            Init();
            ChangeState(cur_state);
        }

        // Update is called once per frame
        void Update()
        {
            Stack_machine.StateUpdate();
            Stack_machine.StateRender();
        }
        [ContextMenu("UI 추가")]
        void Editor_AddUI()
        {
            ChangeState(cur_state);
        }
        [ContextMenu("UI 취소")]
        void Editor_PopUI()
        {
            IState top =Stack_machine.Pop();
           
        }
    }
}
