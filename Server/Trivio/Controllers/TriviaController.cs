using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web.Http;
using System.Web.Http.Description;
using Trivio.Models;

namespace Trivio.Controllers
{
	[RoutePrefix("Trivia")]
    public class TriviaController : ApiController
    {
        private TrivioContext db = new TrivioContext();

		//GET: Trivia/{UserId}
		//POST: Trivia/{UserId}/{Count}
		//PUT: Trivia/Vote

		// GET: api/Trivia
		public IQueryable<Trivia> GetTrivias()
        {
            return db.Trivias;
        }

        // GET: api/Trivia/5
        [ResponseType(typeof(Trivia))]
        public async Task<IHttpActionResult> GetTrivia(int id)
        {
            Trivia trivia = await db.Trivias.FindAsync(id);
            if (trivia == null)
            {
                return NotFound();
            }

            return Ok(trivia);
        }

        // PUT: api/Trivia/5
        [ResponseType(typeof(void))]
        public async Task<IHttpActionResult> PutTrivia(int id, Trivia trivia)
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            if (id != trivia.Id)
            {
                return BadRequest();
            }

            db.Entry(trivia).State = EntityState.Modified;

            try
            {
                await db.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!TriviaExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return StatusCode(HttpStatusCode.NoContent);
        }

        // POST: api/Trivia
        [ResponseType(typeof(Trivia))]
        public async Task<IHttpActionResult> PostTrivia(Trivia trivia)
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            db.Trivias.Add(trivia);
            await db.SaveChangesAsync();

            return CreatedAtRoute("DefaultApi", new { id = trivia.Id }, trivia);
        }

        // DELETE: api/Trivia/5
        [ResponseType(typeof(Trivia))]
        public async Task<IHttpActionResult> DeleteTrivia(int id)
        {
            Trivia trivia = await db.Trivias.FindAsync(id);
            if (trivia == null)
            {
                return NotFound();
            }

            db.Trivias.Remove(trivia);
            await db.SaveChangesAsync();

            return Ok(trivia);
        }


        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }

        private bool TriviaExists(int id)
        {
            return db.Trivias.Count(e => e.Id == id) > 0;
        }
    }
}