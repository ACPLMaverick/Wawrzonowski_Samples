using UnityEngine;
using System.Collections;

public class FluidContainer : MonoBehaviour {

	public double elasticity;
	public float containerBase;
	public float containerHeight;
	
	private SpriteRenderer mySprite;

	public SpriteRenderer MySprite {
		get {
			return mySprite;
		}
	}

	// Use this for initialization
	void Start () {
		mySprite = GetComponent<SpriteRenderer> ();
	}
	
	// Update is called once per frame
	void Update () {
		Vector2 size = new Vector2 (containerBase/10, containerHeight/10);
		transform.localScale = new Vector2 (size.x / 2.5f, size.y / 2.5f);
	}

	public void OnCollisionEnter2D(Collision2D other)
	{
		 
	}
}
