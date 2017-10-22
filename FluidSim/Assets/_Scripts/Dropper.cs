using UnityEngine;
using System.Collections;

public class Dropper : MonoBehaviour
{
    #region variables

    private Vector2 mousePositionInContainer;
    private Vector2 lastMousePosition;
    private bool cursorSwitched;

    #endregion

    #region properties

    public FluidControllerGPU Controller;
    public FluidContainer Container;
    public Texture2D CursorTexture;

    public Vector2 CurrentForcePosition { get; private set; }
    public Vector2 CurrentForceDirection { get; private set; }
    public bool Active { get; set; }

    public float Radius;
    public float ForceValue;
    public float InsertedDensity;
	public float DyeRValue, DyeGValue, DyeBValue;

    #endregion

    #region main

    // Use this for initialization
	void Start () 
    {
        CurrentForceDirection = Vector2.zero;
        CurrentForcePosition = Vector2.zero;
        Active = false;

        //////////////////////////////
	}
	
	// Update is called once per frame
	void Update () 
    {
        Vector3 mouseCoords = Input.mousePosition;
        mouseCoords.z = Container.transform.position.z;

        CurrentForcePosition = Camera.main.ScreenToWorldPoint(mouseCoords);

        Vector2 diff = CurrentForcePosition - lastMousePosition;
        lastMousePosition = CurrentForcePosition;

        CurrentForceDirection = diff;

        //Debug.Log(CurrentForceDirection.x.ToString("00.000000000") + " " + CurrentForceDirection.y.ToString("00.000000000"));

        float divisor = 20.0f;
        bool inBounds = CurrentForcePosition.x < Container.transform.position.x + Container.containerBase / divisor &&
            CurrentForcePosition.x > Container.transform.position.x - Container.containerBase / divisor &&
            CurrentForcePosition.y < Container.transform.position.y + Container.containerHeight / divisor &&
            CurrentForcePosition.y > Container.transform.position.y - Container.containerHeight / divisor;
        if(
            Input.GetMouseButton(0) &&
            inBounds && FluidControllerGPU.Instance.startSimulation
            )
        {
            Active = true;
        }
        else
        {
            Active = false;
        }

        if(inBounds && !cursorSwitched && Controller.startSimulation)
        {
            Cursor.SetCursor(CursorTexture, new Vector2(CursorTexture.width / 2.0f, CursorTexture.height / 2.0f), CursorMode.Auto);
            cursorSwitched = true;
        }
        if (!inBounds && cursorSwitched && Controller.startSimulation)
        {
            Cursor.SetCursor(null, Vector2.zero, CursorMode.Auto);
            cursorSwitched = false;
        }

        //if(cursorSwitched)
        //{
        //    CursorTexture.Resize(64, 64);
        //}
    }

    #endregion
}
