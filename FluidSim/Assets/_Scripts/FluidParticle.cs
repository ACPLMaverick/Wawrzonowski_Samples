using UnityEngine;
using System.Collections;

public class FluidParticle : MonoBehaviour {

	public double mass;
	public double viscosity;
	public float radius;

	public Vector2 force;
	public Vector2 acceleration;
	public Vector2 speed;
	public Vector2 position;
	public Vector2 pressure;

	// Use this for initialization
	void Start () {
        // THIS IS ONLY FOR TEST PURPOSES!!!!!!!!!!!
        this.GetComponent<Renderer>().enabled = false;
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	public void OnCollisionEnter2D(Collision2D other)
	{

	}
}
