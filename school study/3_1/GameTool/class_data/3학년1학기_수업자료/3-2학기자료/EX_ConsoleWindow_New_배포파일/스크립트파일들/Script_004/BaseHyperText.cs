using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System;
using UnityEngine.Events;

namespace Du3Project
{
    public abstract class BaseHyperText : Text, IPointerClickHandler
    {
        Canvas m_RootCanvas;
        Canvas RootCanvas => m_RootCanvas ?? (m_RootCanvas = GetComponentInParent<Canvas>());
        //Canvas RootCanvas
        //{
        //    get { return rootCanvas ?? (rootCanvas = GetComponentInParent<Canvas>()); }
        //}


        // 새롭게 txt가 정리될때 클릭위치들 지정하기 위한것
        protected abstract void AddListeners();

        static readonly BaseObjectPool<List<UIVertex>> m_VerticesPool =
            new BaseObjectPool<List<UIVertex>>(null, l => l.Clear());


        // 비교 문자가 등록되면 문자열이용한 범위 지정용 구조체
        struct Span
        {
            public readonly int StartIndex; // Text 에서 비교된 문자의 시작위치값
            public readonly int Length; // 비교된문자의 크기값
            public readonly Color Color; // 
            public readonly UnityAction<string> Callback; // 콜백 함수
            public List<Rect> BoundingBoxes; // 충돌 처리용 

            public Span(int p_startIndex, int p_endIndex, Color p_color, UnityAction<string> p_callback)
            {
                StartIndex = p_startIndex;
                Length = p_endIndex;
                Color = p_color;
                Callback = p_callback;
                BoundingBoxes = new List<Rect>();
            }
        };


        const int CharVerts = 6; // 하나의 문자가 사용되는 버텍스 갯수
        List<Span> m_Spans = new List<Span>();



        /// <summary>
        /// 버턴 클릭시 액션 호출생성용 함수
        /// </summary>
        /// <param name="startIndex">문자시작 인덱스값</param>
        /// <param name="length">문자 길이값</param>
        /// <param name="color">색상값</param>
        /// <param name="onClick">호출함수</param>
        protected void OnClick(int p_startIndex, int p_length, Color p_color, UnityAction<string> p_onClick)
        {
            if (p_onClick == null)
            {
                throw new ArgumentNullException(nameof(p_onClick));
            }

            if (p_startIndex < 0 || p_startIndex > text.Length - 1)
            {
                throw new ArgumentOutOfRangeException(nameof(p_startIndex));
            }

            if (p_length < 1 || p_startIndex + p_length > text.Length)
            {
                throw new ArgumentOutOfRangeException(nameof(p_length));
            }

            m_Spans.Add(new Span(p_startIndex, p_length, p_color, p_onClick));
        }

        public virtual void RemoveListeners()
        {
            m_Spans.Clear();
        }


        // 2D 위치 이용한 마우스 좌표 월드 좌표 알아내는 방식
        Vector3 CalculateLocalPosition(Vector3 p_position, Camera p_pressEventCamera)
        {
            if (!RootCanvas)
            {
                return Vector3.zero;
            }

            if (RootCanvas.renderMode == RenderMode.ScreenSpaceOverlay)
            {
                return transform.InverseTransformPoint(p_position);
            }

            Vector2 localPosition;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(rectTransform, p_position, p_pressEventCamera, out localPosition);
            return localPosition;
        }


        // 텍스트에서 클릭했을때 호출되는함수
        // 마우스가 클릭된 위치를 기준으로 바운드 박스 사이즈비교해서 바운드 박스 안에 있는 함수를 호출하는 방시
        public void OnPointerClick(PointerEventData p_eventData)
        {
            var localPosition = CalculateLocalPosition(p_eventData.position, p_eventData.pressEventCamera);

            for (var s = 0; s < m_Spans.Count; s++)
            {
                for (var b = 0; b < m_Spans[s].BoundingBoxes.Count; b++)
                {
                    if (m_Spans[s].BoundingBoxes[b].Contains(localPosition))
                    {
                        m_Spans[s].Callback(text.Substring(m_Spans[s].StartIndex, m_Spans[s].Length));
                        break;
                    }
                }
            }
        }



        // 버텍스 정보 이용한 위치 지정용 함수
        // ui로 사용할 매쉬 생성용 VertexHelper 적용
        protected override void OnPopulateMesh(VertexHelper p_vertexHelper)
        {
            base.OnPopulateMesh(p_vertexHelper);

            m_Spans.Clear();
            AddListeners();

            var vertices = m_VerticesPool.Get();
            p_vertexHelper.GetUIVertexStream(vertices);

            Modify(ref vertices);

            p_vertexHelper.Clear();
            p_vertexHelper.AddUIVertexTriangleStream(vertices);
            m_VerticesPool.Release(vertices);
        }


        // ui에서 사용할 사이즈 지정용 vertex
        // text 에서 사용하는 각 단어별 Vertextbuffer 에 Color 값을 직접 적용하는 방법임
        void Modify(ref List<UIVertex> p_vertices)
        {
            var verticesCount = p_vertices.Count;

            for (var i = 0; i < m_Spans.Count; i++)
            {
                var span = m_Spans[i];
                var endIndex = span.StartIndex + span.Length;

                // 전체 단어가 시작하는 곳부터 몇단어까지 떨어졌는지 비교해서 직접 문자색상값 바꾸도록 처리함
                for (var textIndex = span.StartIndex; textIndex < endIndex; textIndex++)
                {
                    var vertexStartIndex = textIndex * CharVerts;
                    if (vertexStartIndex + CharVerts > verticesCount)
                    {
                        break;
                    }

                    var min = Vector2.one * float.MaxValue;
                    var max = Vector2.one * float.MinValue;

                    for (var vertexIndex = 0; vertexIndex < CharVerts; vertexIndex++)
                    {
                        var vertex = p_vertices[vertexStartIndex + vertexIndex];
                        vertex.color = span.Color;
                        p_vertices[vertexStartIndex + vertexIndex] = vertex;

                        var pos = p_vertices[vertexStartIndex + vertexIndex].position;

                        if (pos.y < min.y)
                        {
                            min.y = pos.y;
                        }

                        if (pos.x < min.x)
                        {
                            min.x = pos.x;
                        }

                        if (pos.y > max.y)
                        {
                            max.y = pos.y;
                        }

                        if (pos.x > max.x)
                        {
                            max.x = pos.x;
                        }
                    }

                    span.BoundingBoxes.Add(new Rect { min = min, max = max });
                }

                // 바운딩 박스사이즈 알아내기
                span.BoundingBoxes = CalculateLineBoundingBoxes(span.BoundingBoxes);
                m_Spans[i] = span;
            }
        }

        static List<Rect> CalculateLineBoundingBoxes(List<Rect> p_charBoundingBoxes)
        {
            var lineBoundingBoxes = new List<Rect>();
            var lineStartIndex = 0;

            for (var i = 1; i < p_charBoundingBoxes.Count; i++)
            {
                if (p_charBoundingBoxes[i].xMin >= p_charBoundingBoxes[i - 1].xMin)
                {
                    continue;
                }

                lineBoundingBoxes.Add(CalculateAABB(p_charBoundingBoxes.GetRange(lineStartIndex, i - lineStartIndex)));
                lineStartIndex = i;
            }

            if (lineStartIndex < p_charBoundingBoxes.Count)
            {
                lineBoundingBoxes.Add(CalculateAABB(p_charBoundingBoxes.GetRange(lineStartIndex, p_charBoundingBoxes.Count - lineStartIndex)));
            }

            return lineBoundingBoxes;
        }


        // 문자들의 위치들 알아내서 2D박스 사이즈 만들기
        static Rect CalculateAABB(IReadOnlyList<Rect> p_rects)
        {
            var min = Vector2.one * float.MaxValue;
            var max = Vector2.one * float.MinValue;

            for (var i = 0; i < p_rects.Count; i++)
            {
                if (p_rects[i].xMin < min.x)
                {
                    min.x = p_rects[i].xMin;
                }

                if (p_rects[i].yMin < min.y)
                {
                    min.y = p_rects[i].yMin;
                }

                if (p_rects[i].xMax > max.x)
                {
                    max.x = p_rects[i].xMax;
                }

                if (p_rects[i].yMax > max.y)
                {
                    max.y = p_rects[i].yMax;
                }
            }

            return new Rect { min = min, max = max };
        }


    }
}