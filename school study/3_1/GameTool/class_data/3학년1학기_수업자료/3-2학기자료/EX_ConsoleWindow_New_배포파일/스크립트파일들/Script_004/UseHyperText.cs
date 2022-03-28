using System;
using System.Collections;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.Events;

namespace Du3Project
{
    public class UseHyperText : BaseHyperText
    {
        struct Entry
        {
            public readonly string RegexPattern;
            public readonly Color Color;
            public readonly UnityAction<string> Callback;

            public Entry(string p_regexPattern, Color p_color, UnityAction<string> p_callback)
            {
                RegexPattern = p_regexPattern;
                Color = p_color;
                Callback = p_callback;
            }
        }

        readonly List<Entry> m_Entries = new List<Entry>();


        // 새롭게 txt가 정리될때 클릭위치들 지정하기 위한것
        protected override void AddListeners()
        {
            if (text.Length <= 0)
                return;

            foreach (var entry in m_Entries)
            {
                foreach (Match match in Regex.Matches(text, entry.RegexPattern))
                {
                    OnClick(match.Index, match.Value.Length, entry.Color, entry.Callback);
                }
            }

        }

        // regexPattern 일치하는 부분 문자열에 클릭 이벤트를 등록
        public void OnClick(string p_regexPattern, UnityAction<string> p_onClick)
        {
            // color은 현재 text의 기본 색상 사용하기
            OnClick(p_regexPattern, color, p_onClick);
        }

        // regexPattern 일치하는 부분 문자열과 색상 지정 클릭 이벤트를 등록
        public void OnClick(string p_regexPattern, Color p_color, UnityAction<string> p_onClick)
        {
            if (string.IsNullOrEmpty(p_regexPattern) || p_onClick == null)
            {
                return;
            }

            m_Entries.Add(new Entry(p_regexPattern, p_color, p_onClick));
        }

        public override void RemoveListeners()
        {
            base.RemoveListeners();
            m_Entries.Clear();
        }


    }
}